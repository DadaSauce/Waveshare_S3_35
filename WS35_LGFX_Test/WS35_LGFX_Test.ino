/*
  =============================================================================
   Waveshare ESP32-S3-Touch-LCD-3.5 - LovyanGFX test: speed, screen, touch
  =============================================================================
  Same board pins as WS35_Test:
    LCD   ST7796 320x480 IPS, SPI: SCLK GPIO5, MOSI GPIO1, MISO GPIO2,
          DC GPIO3, backlight GPIO6 (PWM), CS tied on, RESET via IO expander
    I2C   SDA GPIO8, SCL GPIO7: touch FT6336 0x38, expander TCA9554 0x20, ...

  LIBRARY:  "LovyanGFX" (by lovyan03) - Library Manager.
  BOARD:    "ESP32S3 Dev Module", USB CDC On Boot: Enabled,
            Flash Size: 16MB, PSRAM: OPI PSRAM   (PSRAM is needed!)

  WHAT IT DOES (Serial Monitor shows the numbers)
    1. Full screen red, green, blue, white - 0.5 s each
    2. Speed test: a direct full-screen fill, and a full frame drawn
       off-screen in PSRAM then sent in one burst (DMA) - ms and frames/s
    3. Test screen: frame, labelled color bars, grey ramp; draw with a finger,
       CLEAR box at the bottom
  COMMANDS
    anim        5 s of bouncing balls, drawn off-screen: shows the real fps
    bench       repeat the speed test
    inv         invert colors          rgb   swap red/blue
    bl <0-255>  backlight brightness
    ofs <x> <y> shift the picture by whole pixels (e.g. ofs 2 0), to find
                out if a missing edge is an offset or just covered by the bezel
  =============================================================================
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>

const int PIN_SDA = 8, PIN_SCL = 7;
const uint8_t TOUCH_ADDR = 0x38;        // FT6336
const uint8_t EXP_ADDR   = 0x20;        // TCA9554 (found by WS35_Test)

// ---- LovyanGFX setup for this board ----
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel;
  lgfx::Bus_SPI      _bus;
  lgfx::Light_PWM    _light;
public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host   = SPI2_HOST;
      cfg.spi_mode   = 0;
      cfg.freq_write = 80000000;        // 80 MHz worked cleanly on this board
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = false;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 5;
      cfg.pin_mosi = 1;
      cfg.pin_miso = 2;
      cfg.pin_dc   = 3;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    {
      auto cfg = _panel.config();
      cfg.pin_cs   = -1;                // tied on
      cfg.pin_rst  = -1;                // reset goes through the IO expander
      cfg.pin_busy = -1;
      cfg.panel_width  = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.readable   = true;
      cfg.invert     = true;            // IPS panel
      cfg.rgb_order  = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;
      _panel.config(cfg);
    }
    {
      auto cfg = _light.config();
      cfg.pin_bl = 6;
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;
      _light.config(cfg);
      _panel.setLight(&_light);
    }
    setPanel(&_panel);
  }
  // swap red/blue: change the panel setting and start it again
  void setRGBOrder(bool rgb) {
    auto cfg = _panel.config();
    cfg.rgb_order = rgb;
    _panel.config(cfg);
  }
  void setOffsets(int x, int y) {
    auto cfg = _panel.config();
    cfg.offset_x = x;
    cfg.offset_y = y;
    _panel.config(cfg);
  }
  void setInvertCfg(bool inv) {
    auto cfg = _panel.config();
    cfg.invert = inv;
    _panel.config(cfg);
  }
};

LGFX lcd;
LGFX_Sprite frame(&lcd);                // full-screen off-screen buffer (PSRAM)
bool haveFrame = false;
bool invertOn = true, rgbOrder = false;
int  ofsX = 0, ofsY = 0;

// ---- I2C helpers (expander + touch) ----
void i2cWrite(uint8_t a, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(a); Wire.write(reg); Wire.write(val); Wire.endTransmission();
}
int i2cRead(uint8_t a, uint8_t reg, uint8_t* buf, int n) {
  Wire.beginTransmission(a); Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  int got = Wire.requestFrom((int)a, n);
  for (int i = 0; i < got; i++) buf[i] = Wire.read();
  return got;
}

// TCA9554: reg 1 = outputs, reg 3 = direction (1 = input)
uint8_t expOut = 0x7F;                  // all high except P7 (speaker amp off)
void expanderPin(int pin, bool high) {
  if (high) expOut |= (1 << pin); else expOut &= ~(1 << pin);
  i2cWrite(EXP_ADDR, 1, expOut);
}
void lcdHardReset() {
  i2cWrite(EXP_ADDR, 1, expOut);
  i2cWrite(EXP_ADDR, 3, 0b01110100);    // P0,P1,P3,P7 outputs
  expanderPin(1, true);  delay(10);
  expanderPin(1, false); delay(20);
  expanderPin(1, true);  delay(120);
}

int readTouch(int* xs, int* ys) {       // FT6336, up to 2 fingers
  uint8_t d[11];
  if (i2cRead(TOUCH_ADDR, 0x02, d, 11) != 11) return 0;
  int n = d[0] & 0x0F;
  if (n > 2) n = 0;
  for (int i = 0; i < n; i++) {
    const uint8_t* p = d + 1 + i * 6;
    xs[i] = ((p[0] & 0x0F) << 8) | p[1];
    ys[i] = ((p[2] & 0x0F) << 8) | p[3];
  }
  return n;
}

// ---- tests ----
void benchmark() {
  unsigned long t0 = micros();
  for (int i = 0; i < 4; i++) lcd.fillScreen(i & 1 ? TFT_BLACK : TFT_NAVY);
  unsigned long direct = (micros() - t0) / 4;
  Serial.printf("direct full-screen fill:      %3lu ms  (%lu fps)\n",
                direct / 1000, direct ? 1000000UL / direct : 0);
  if (haveFrame) {
    frame.fillScreen(TFT_DARKGREEN);
    frame.setTextColor(TFT_WHITE);
    frame.setTextSize(3);
    frame.drawString("off-screen frame", 20, 220);
    t0 = micros();
    for (int i = 0; i < 4; i++) frame.pushSprite(0, 0);
    unsigned long push = (micros() - t0) / 4;
    Serial.printf("off-screen frame, one burst:  %3lu ms  (%lu fps)\n",
                  push / 1000, push ? 1000000UL / push : 0);
  }
}

const int CLEAR_Y = 440;

void drawTestScreen() {
  int w = lcd.width(), h = lcd.height();
  lcd.startWrite();
  lcd.fillScreen(TFT_BLACK);
  lcd.drawRect(0, 0, w, h, TFT_WHITE);
  lcd.setTextColor(TFT_WHITE);
  lcd.setTextSize(2);
  lcd.drawString("ESP32-S3 3.5\" LovyanGFX", 10, 10);
  const uint16_t cols[7] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_CYAN, TFT_MAGENTA, TFT_YELLOW, TFT_WHITE };
  const char* names = "RGBCMYW";
  int bw = (w - 20) / 7;
  for (int i = 0; i < 7; i++) {
    lcd.fillRect(10 + i * bw, 40, bw, 90, cols[i]);
    lcd.setTextColor(TFT_BLACK);
    char s[2] = { names[i], 0 };
    lcd.drawString(s, 10 + i * bw + bw / 2 - 5, 105);
  }
  for (int x = 10; x < w - 10; x++) {
    uint8_t v = (x - 10) * 255 / (w - 21);
    lcd.drawFastVLine(x, 140, 20, lcd.color565(v, v, v));
  }
  lcd.setTextColor(TFT_DARKGREY);
  lcd.drawString("Touch to draw", 10, 180);
  // edge markers: a 1-px line in a different color on each side, 3 px in
  lcd.drawFastVLine(3, 3, h - 6, TFT_RED);        // left   = red
  lcd.drawFastVLine(w - 4, 3, h - 6, TFT_GREEN);  // right  = green
  lcd.drawFastHLine(3, 3, w - 6, TFT_YELLOW);     // top    = yellow
  lcd.drawFastHLine(3, h - 4, w - 6, TFT_CYAN);   // bottom = cyan
  char ofsTxt[24];
  snprintf(ofsTxt, sizeof(ofsTxt), "ofs %d,%d", ofsX, ofsY);
  lcd.setTextColor(TFT_DARKGREY);
  lcd.drawString(ofsTxt, 10, 400);
  lcd.drawRect(w / 2 - 60, CLEAR_Y, 120, 30, TFT_ORANGE);
  lcd.setTextColor(TFT_ORANGE);
  lcd.drawString("CLEAR", w / 2 - 29, CLEAR_Y + 8);
  lcd.endWrite();
}

// bouncing balls drawn into the off-screen frame, then sent in one burst
void anim() {
  if (!haveFrame) { Serial.println("no PSRAM frame - set PSRAM: OPI PSRAM"); return; }
  const int N = 12;
  float x[N], y[N], vx[N], vy[N]; uint16_t c[N];
  for (int i = 0; i < N; i++) {
    x[i] = random(20, 300); y[i] = random(20, 460);
    vx[i] = random(-60, 60) / 10.0f + 1; vy[i] = random(-60, 60) / 10.0f + 1;
    c[i] = lcd.color565(random(80, 255), random(80, 255), random(80, 255));
  }
  unsigned long start = millis(), lastFps = millis();
  int frames = 0, fps = 0;
  while (millis() - start < 5000) {
    frame.fillScreen(TFT_BLACK);
    for (int i = 0; i < N; i++) {
      x[i] += vx[i]; y[i] += vy[i];
      if (x[i] < 14 || x[i] > 306) vx[i] = -vx[i];
      if (y[i] < 14 || y[i] > 466) vy[i] = -vy[i];
      frame.fillCircle((int)x[i], (int)y[i], 14, c[i]);
    }
    frame.setTextColor(TFT_WHITE, TFT_BLACK);
    frame.setTextSize(2);
    frame.drawString(String(fps) + " fps", 8, 8);
    frame.pushSprite(0, 0);
    frames++;
    if (millis() - lastFps >= 1000) { fps = frames; frames = 0; lastFps = millis(); Serial.printf("anim: %d fps\n", fps); }
  }
  drawTestScreen();
}

void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000);
  Serial.println("\nWaveshare 3.5 - LovyanGFX test");

  Wire.begin(PIN_SDA, PIN_SCL, 400000);
  lcdHardReset();

  lcd.init();
  lcd.setBrightness(255);

  // off-screen frame in PSRAM (320 x 480 x 2 bytes = 300 KB)
  frame.setPsram(true);
  frame.setColorDepth(16);
  haveFrame = frame.createSprite(320, 480) != nullptr;
  Serial.println(haveFrame ? "PSRAM frame ready" : "PSRAM frame FAILED - enable OPI PSRAM");

  const uint16_t full[4] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_WHITE };
  const char* names[4] = { "RED", "GREEN", "BLUE", "WHITE" };
  for (int i = 0; i < 4; i++) {
    Serial.printf("screen should be %s\n", names[i]);
    lcd.fillScreen(full[i]);
    delay(500);
  }
  benchmark();
  drawTestScreen();
  Serial.println("Ready - touch to draw. Commands: anim | bench | inv | rgb | bl <0-255> | ofs <x> <y>");
}

void loop() {
  if (Serial.available()) {
    String in = Serial.readStringUntil('\n');
    in.trim(); in.toLowerCase();
    if (in == "anim") anim();
    else if (in == "bench") { benchmark(); drawTestScreen(); }
    else if (in == "inv") { invertOn = !invertOn; lcd.invertDisplay(invertOn); Serial.println(invertOn ? "invert on" : "invert off"); }
    else if (in == "rgb") {
      rgbOrder = !rgbOrder;
      lcd.setRGBOrder(rgbOrder);
      lcd.setRotation(lcd.getRotation());   // re-sends the color order - no restart needed
      drawTestScreen();
      Serial.println(rgbOrder ? "rgb_order = true" : "rgb_order = false");
    }
    else if (in.startsWith("ofs")) {
      String a = in.substring(3); a.trim();
      int sp = a.indexOf(' ');
      if (sp < 0) { Serial.println("usage: ofs <x> <y>  e.g. ofs 2 0"); return; }
      ofsX = constrain(a.substring(0, sp).toInt(), -8, 8);
      ofsY = constrain(a.substring(sp + 1).toInt(), -8, 8);
      lcd.setOffsets(ofsX, ofsY);
      lcd.setRotation(lcd.getRotation());   // re-applies offsets - no restart needed
      drawTestScreen();
      Serial.printf("offset %d,%d\n", ofsX, ofsY);
    }
    else if (in.startsWith("bl")) { int b = constrain(in.substring(2).toInt(), 0, 255); lcd.setBrightness(b); Serial.printf("backlight %d\n", b); }
  }

  int xs[2], ys[2];
  int n = readTouch(xs, ys);
  for (int i = 0; i < n; i++) {
    if (ys[i] >= CLEAR_Y && ys[i] <= CLEAR_Y + 30 && abs(xs[i] - lcd.width() / 2) < 60) {
      drawTestScreen();
      delay(300);
      return;
    }
    lcd.fillCircle(xs[i], ys[i], 4, i == 0 ? TFT_CYAN : TFT_MAGENTA);
  }
  delay(5);
}
