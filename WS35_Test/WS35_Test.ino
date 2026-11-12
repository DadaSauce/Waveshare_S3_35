/*
  =============================================================================
   Waveshare ESP32-S3-Touch-LCD-3.5 - first test: display, touch, I2C bus
  =============================================================================
  Board hardware (from Waveshare's schematic):
    LCD   ST7796 320x480 IPS, SPI: SCLK GPIO5, MOSI GPIO1, MISO GPIO2,
          DC GPIO3, backlight GPIO6, CS tied on, RESET via the IO expander
    I2C   SDA GPIO8, SCL GPIO7 - shared by touch (FT6336), IO expander
          (TCA9554), RTC (PCF85063), IMU (QMI8658), audio (ES8311), PMU (AXP2101)
    TCA9554 expander: P0 cam power-down, P1 LCD reset, P2 touch INT,
          P3 SD CS, P4 RTC INT, P5 PMU IRQ, P6 SYS_OUT, P7 speaker amp

  LIBRARY:  "GFX Library for Arduino" (by moononournation) - Library Manager.
  BOARD:    esp32 core -> "ESP32S3 Dev Module", with
              USB CDC On Boot: Enabled      (Serial over the USB-C port)
              Flash Size: 16MB              PSRAM: OPI PSRAM
              Partition Scheme: 16M Flash (3MB APP/9.9MB FATFS)  (or default)

  WHAT IT DOES
    - Serial Monitor (115200): lists every I2C chip found (should show
      0x20-ish expander, 0x38 touch, 0x51 RTC, 0x6A/0x6B IMU, 0x34 PMU, 0x18 audio)
    - Resets the LCD through the expander, turns on the backlight
    - Test screen: white frame on the edge, labelled color bars R G B C M Y W
    - Touch anywhere: draws a dot and prints x,y. Two fingers work too.
    - Touch the "CLEAR" box (bottom) to wipe the drawing.

  If colors look wrong, Serial commands: inv (invert), bgr (swap red/blue)
  =============================================================================
*/

#include <Wire.h>
#include <Arduino_GFX_Library.h>

// ---- pins ----
const int PIN_SCLK = 5, PIN_MOSI = 1, PIN_MISO = 2, PIN_DC = 3, PIN_BL = 6;
const int PIN_SDA = 8, PIN_SCL = 7;
const uint8_t TOUCH_ADDR = 0x38;        // FT6336

// ---- display ----
// Arduino_HWSPI uses the ESP32 core's own SPI driver - the most compatible
// choice with esp32 core 3.x (Arduino_ESP32SPI stayed black on this setup)
Arduino_DataBus* bus = new Arduino_HWSPI(PIN_DC, GFX_NOT_DEFINED /*CS*/, PIN_SCLK, PIN_MOSI, PIN_MISO, &SPI, false);
Arduino_GFX* gfx = new Arduino_ST7796(bus, GFX_NOT_DEFINED /*RST via expander*/, 0 /*rotation*/, true /*IPS*/, 320, 480);

uint8_t expAddr = 0;                    // TCA9554, found by scanning
int spiMHz = 40;                        // "spd 20|40|60|80" changes it live
bool inverted = false, bgr = false;

// ---- I2C helpers ----
bool i2cPresent(uint8_t a) {
  Wire.beginTransmission(a);
  return Wire.endTransmission() == 0;
}

void i2cWrite(uint8_t a, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(a);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

int i2cRead(uint8_t a, uint8_t reg, uint8_t* buf, int n) {
  Wire.beginTransmission(a);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  int got = Wire.requestFrom((int)a, n);
  for (int i = 0; i < got; i++) buf[i] = Wire.read();
  return got;
}

void scanI2C() {
  Serial.println("I2C devices:");
  for (uint8_t a = 1; a < 127; a++) {
    if (!i2cPresent(a)) continue;
    const char* what = "";
    if (a >= 0x20 && a <= 0x27) what = "TCA9554 IO expander";
    else if (a == 0x38) what = "FT6336 touch";
    else if (a == 0x51) what = "PCF85063 RTC";
    else if (a == 0x6A || a == 0x6B) what = "QMI8658 IMU";
    else if (a == 0x34) what = "AXP2101 power";
    else if (a == 0x18) what = "ES8311 audio";
    else if (a == 0x3C) what = "camera (SCCB)";
    Serial.printf("  0x%02X  %s\n", a, what);
    if (a >= 0x20 && a <= 0x27 && expAddr == 0) expAddr = a;
  }
}

// TCA9554 registers: 1 = output, 3 = config (1 = input, 0 = output)
uint8_t expOut = 0xFF;
void expanderInit() {
  if (!expAddr) { Serial.println("No IO expander found - LCD reset skipped"); return; }
  expOut = 0xFF;                        // all high (LCD not in reset, amp off-ish)
  expOut &= ~(1 << 7);                  // speaker amp off
  i2cWrite(expAddr, 1, expOut);
  // outputs: P0 cam pwdn, P1 LCD reset, P3 SD CS, P7 amp. Inputs: P2, P4, P5, P6
  i2cWrite(expAddr, 3, 0b01110100);
}

void expanderPin(int pin, bool high) {
  if (!expAddr) return;
  if (high) expOut |= (1 << pin); else expOut &= ~(1 << pin);
  i2cWrite(expAddr, 1, expOut);
}

void lcdReset() {
  expanderPin(1, true);  delay(10);
  expanderPin(1, false); delay(20);
  expanderPin(1, true);  delay(120);
}

// ---- touch (FT6336): up to 2 points ----
int readTouch(int* xs, int* ys) {
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

// ---- test screen ----
const int CLEAR_Y = 440;

void drawTestScreen() {
  int w = gfx->width(), h = gfx->height();
  gfx->fillScreen(RGB565_BLACK);
  gfx->drawRect(0, 0, w, h, RGB565_WHITE);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(10, 10);
  gfx->print("ESP32-S3 3.5\" ST7796");
  gfx->setTextSize(1);
  gfx->setCursor(10, 32);
  gfx->printf("%dx%d  expander 0x%02X", w, h, expAddr);

  const uint16_t cols[7] = { RGB565_RED, RGB565_GREEN, RGB565_BLUE, RGB565_CYAN,
                             RGB565_MAGENTA, RGB565_YELLOW, RGB565_WHITE };
  const char* names = "RGBCMYW";
  int bw = (w - 20) / 7;
  for (int i = 0; i < 7; i++) {
    gfx->fillRect(10 + i * bw, 50, bw, 90, cols[i]);
    gfx->setTextColor(RGB565_BLACK);
    gfx->setTextSize(2);
    gfx->setCursor(10 + i * bw + bw / 2 - 5, 115);
    gfx->print(names[i]);
  }
  for (int x = 10; x < w - 10; x++) {          // grey ramp
    uint8_t v = (x - 10) * 255 / (w - 21);
    gfx->drawFastVLine(x, 150, 20, gfx->color565(v, v, v));
  }
  gfx->setTextColor(RGB565_DARKGREY);
  gfx->setTextSize(2);
  gfx->setCursor(10, 190);
  gfx->print("Touch to draw");
  gfx->drawRect(w / 2 - 60, CLEAR_Y, 120, 30, RGB565_ORANGE);
  gfx->setTextColor(RGB565_ORANGE);
  gfx->setCursor(w / 2 - 29, CLEAR_Y + 8);
  gfx->print("CLEAR");
}

void applyColorOptions() {
  gfx->invertDisplay(inverted);
  // memory-access control: rotation 0 plus the red/blue order bit
  bus->beginWrite();
  bus->writeC8D8(0x36, 0x48 ^ (bgr ? 0x08 : 0x00));
  bus->endWrite();
}

// time a full-screen fill (320x480 = 300 KB of pixels)
void benchmark() {
  unsigned long t0 = micros();
  for (int i = 0; i < 4; i++) gfx->fillScreen(i & 1 ? RGB565_BLACK : RGB565_NAVY);
  unsigned long us = (micros() - t0) / 4;
  Serial.printf("%d MHz: full-screen fill %lu ms (about %lu frames/s)\n",
                spiMHz, us / 1000, us ? 1000000UL / us : 0);
}

void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000);
  Serial.println("\nWaveshare ESP32-S3-Touch-LCD-3.5 test");

  Wire.begin(PIN_SDA, PIN_SCL, 400000);
  scanI2C();
  expanderInit();
  lcdReset();

  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);           // backlight on

  if (!gfx->begin(spiMHz * 1000000L)) Serial.println("gfx->begin() failed");
  else Serial.printf("gfx->begin() ok at %d MHz\n", spiMHz);

  // unmistakable test: whole screen red, green, blue, white
  const uint16_t full[4] = { RGB565_RED, RGB565_GREEN, RGB565_BLUE, RGB565_WHITE };
  const char* names[4] = { "RED", "GREEN", "BLUE", "WHITE" };
  for (int i = 0; i < 4; i++) {
    Serial.printf("screen should be %s\n", names[i]);
    gfx->fillScreen(full[i]);
    delay(1000);
  }
  benchmark();
  drawTestScreen();
  Serial.println("Ready - touch the screen. Commands: inv | bgr | spd 20|40|60|80 | bench");
}

void loop() {
  // serial commands
  if (Serial.available()) {
    String in = Serial.readStringUntil('\n');
    in.trim(); in.toLowerCase();
    if (in == "inv") { inverted = !inverted; gfx->invertDisplay(inverted); Serial.println(inverted ? "inverted" : "normal"); }
    else if (in == "bgr") { bgr = !bgr; applyColorOptions(); drawTestScreen(); Serial.println(bgr ? "BGR" : "RGB"); }
    else if (in.startsWith("spd")) {
      int m = in.substring(3).toInt();
      if (m < 10 || m > 80) { Serial.println("spd 10..80 (MHz)"); return; }
      spiMHz = m;
      gfx->begin(spiMHz * 1000000L);   // re-start the bus at the new speed
      gfx->invertDisplay(inverted);
      benchmark();
      drawTestScreen();
    }
    else if (in == "bench") { benchmark(); drawTestScreen(); }
  }

  // touch
  int xs[2], ys[2];
  int n = readTouch(xs, ys);
  static unsigned long lastPrint = 0;
  for (int i = 0; i < n; i++) {
    int x = xs[i], y = ys[i];
    if (y >= CLEAR_Y && y <= CLEAR_Y + 30 && abs(x - gfx->width() / 2) < 60) {
      drawTestScreen();
      delay(300);
      return;
    }
    gfx->fillCircle(x, y, 4, i == 0 ? RGB565_CYAN : RGB565_MAGENTA);
    if (millis() - lastPrint > 100) { Serial.printf("touch %d: %d,%d\n", i, x, y); lastPrint = millis(); }
  }
  delay(10);
}
