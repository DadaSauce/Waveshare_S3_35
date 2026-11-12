/*
  =============================================================================
   Jump-hour retrograde clock - Waveshare ESP32-S3-Touch-LCD-3.5 (480x320)
  =============================================================================
  Same idea as the XIAO version, bigger and smoother:
    - minute hand sweeps a 116-degree fan 00 -> 60, then snaps back (animated)
      at the top of the hour while the hour digits roll to the next hour
    - cyan seconds track fills over each minute
    - weekday bottom-left, month + day bottom-right
  The face ignores touches except on the gear (status: Serial "status").
  Gear (top right): settings - set the time, set the alarm, alarm on/off,
  test the beep. The alarm beeps for 15 s; touching the screen stops it.
  When the alarm is on, a bell and its time show top-left.
  (Beeps need a small speaker on the board's speaker header, MX1.25.)

  TIME (like the stepper clock)
    - Wi-Fi is only on to sync: at power-up and daily at 3:17 AM, then off.
      A failed sync retries hourly (every 5 min if there's no time at all).
    - US Pacific time with DST, from NTP.
    - The onboard PCF85063 RTC keeps time between syncs and through power
      cuts (fit a backup cell on the RTC battery header for that), so the
      right time shows the moment it powers up - before Wi-Fi connects.
    - Wi-Fi name/password are NOT in this sketch: send them once over USB
      (Serial Monitor) and they're saved in the board's flash.

  LIBRARY: "LovyanGFX" (by lovyan03).
  BOARD:   "ESP32S3 Dev Module", USB CDC On Boot: Enabled, Flash Size: 16MB,
           PSRAM: OPI PSRAM.

  SERIAL MONITOR (115200, "Newline")
    ssid <name>       Wi-Fi network name (saved)
    pass <password>   Wi-Fi password (saved; never printed)
    sync              sync now
    status            time, Wi-Fi, last/next sync, RTC
    12h / 24h         hour window style (saved)
    bl <0-255>        backlight brightness (saved)
    alarm HH:MM       set the alarm (and turn it on); alarm on / off / test
    demo              show the hour change now (real time comes back after)
    help
  =============================================================================
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "ESP_I2S.h"
using namespace lgfx::textdatum;       // middle_center, bottom_left, ...

// ---------------- board ----------------
const int PIN_SDA = 8, PIN_SCL = 7;
const uint8_t TOUCH_ADDR = 0x38;        // FT6336
const uint8_t EXP_ADDR   = 0x20;        // TCA9554 IO expander
const uint8_t RTC_ADDR   = 0x51;        // PCF85063

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel;
  lgfx::Bus_SPI      _bus;
  lgfx::Light_PWM    _light;
public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 80000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 5; cfg.pin_mosi = 1; cfg.pin_miso = 2; cfg.pin_dc = 3;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    {
      auto cfg = _panel.config();
      cfg.pin_cs = -1; cfg.pin_rst = -1; cfg.pin_busy = -1;
      cfg.panel_width = 320; cfg.panel_height = 480;
      cfg.offset_x = 0; cfg.offset_y = 0; cfg.offset_rotation = 0;
      cfg.readable = true; cfg.invert = true; cfg.rgb_order = false;
      cfg.dlen_16bit = false; cfg.bus_shared = false;
      _panel.config(cfg);
    }
    {
      auto cfg = _light.config();
      cfg.pin_bl = 6; cfg.invert = false; cfg.freq = 44100; cfg.pwm_channel = 7;
      _light.config(cfg);
      _panel.setLight(&_light);
    }
    setPanel(&_panel);
  }
};

// on-screen button
struct Btn { int id, x, y, w, h; };

LGFX lcd;
LGFX_Sprite frame(&lcd);                // 480x320 off-screen frame in PSRAM
const int W = 480, H = 320;

// ---------------- face design ----------------
const uint16_t C_BG     = TFT_BLACK;
const uint16_t C_ARC    = 0x4208;       // dark grey
const uint16_t C_TICK   = 0x8410;       // grey minute ticks
const uint16_t C_TICK5  = 0xFFFF;       // white 5-minute ticks
const uint16_t C_LABEL  = 0xDEFB;       // light grey labels
const uint16_t C_HAND   = 0xFFFF;       // white hand
const uint16_t C_SECON  = 0x07FF;       // cyan seconds
const uint16_t C_SECOFF = 0x2104;       // dark track
const uint16_t C_HOUR   = 0xFD80;       // amber hour digits
const uint16_t C_WINRIM = 0x8410;
const uint16_t C_DATE   = 0x2D7F;       // sky blue date
const uint16_t C_OK     = 0x07E0, C_BAD = 0xF800, C_CARD = 0x18E3;

const int   PX = 240, PY = 222;         // pivot, just inside the window top
const float SPAN = 58;                  // fan from -58 deg (00) to +58 deg (60)
const float R_ARC = 196, R_T5 = 168, R_T1 = 180, R_TOUT = 192, R_LABEL = 207;
const float R_S_IN = 126, R_S_OUT = 140;
const float R_HAND0 = 26, R_HAND = 160, W_HAND = 12;
const int   WIN_W = 150, WIN_H = 96, WIN_X = PX - WIN_W / 2, WIN_Y = 218;

// ---------------- settings / state ----------------
Preferences prefs;
String wifiSsid, wifiPass;
bool hour12 = false;
int  backlight = 220;

const char* TZ_INFO = "PST8PDT,M3.2.0,M11.1.0";   // US Pacific with DST
const char* NTP1 = "pool.ntp.org";
const char* NTP2 = "time.nist.gov";
const int SYNC_HOUR = 3, SYNC_MIN = 17;
const unsigned long SYNC_TIMEOUT_MS = 60000;

volatile bool sntpDone = false;
bool wifiSyncing = false, ntpStarted = false, lastSyncFailed = false;
unsigned long syncStart = 0;
time_t lastSyncOk = 0;
int lastSyncDay = -1, lastAttemptHour = -1;
bool rtcFound = false, rtcValidAtBoot = false;

// demo: show hh:59:55, hour change at 5 s, back to real time at 8 s
bool demoActive = false;
long demoOffset = 0;                    // seconds added to the real time
unsigned long demoStart = 0;

unsigned long cardUntil = 0;            // status card shown until this millis()

int shownHour = -1, shownSec = -1;      // what the face shows now
float shownHand = 0;

// ---- screens, settings, alarm ----
const int SCREEN_FACE = 0, SCREEN_SETTINGS = 1;
int screen = SCREEN_FACE;
unsigned long lastTouchMs = 0;          // settings close after 60 s untouched

int  editH = 0, editM = 0;              // "set time" fields
int  alarmH = 7, alarmM = 0;            // saved alarm
bool alarmOn = false;
unsigned long savedMsgUntil = 0;        // "Time set" / "Saved" note
const char* savedMsg = "";

volatile bool alarmRinging = false;     // the beep task plays while this is true
unsigned long alarmStartMs = 0, alarmLimitMs = 15000;
long lastAlarmKey = -1;                 // minute the alarm last went off (no repeats)

// button ids
enum { B_NONE = 0, B_DONE, B_TH_UP, B_TH_DN, B_TM_UP, B_TM_DN, B_SET,
       B_AH_UP, B_AH_DN, B_AM_UP, B_AM_DN, B_AON, B_TEST };
const Btn BTNS[] = {
  { B_DONE, 370, 8, 100, 44 },
  { B_TH_UP, 35, 92, 70, 44 },  { B_TH_DN, 35, 206, 70, 44 },
  { B_TM_UP, 135, 92, 70, 44 }, { B_TM_DN, 135, 206, 70, 44 },
  { B_SET, 55, 268, 130, 44 },
  { B_AH_UP, 275, 92, 70, 44 }, { B_AH_DN, 275, 206, 70, 44 },
  { B_AM_UP, 375, 92, 70, 44 }, { B_AM_DN, 375, 206, 70, 44 },
  { B_AON, 262, 268, 100, 44 }, { B_TEST, 372, 268, 96, 44 },
};
const int NBTN = sizeof(BTNS) / sizeof(BTNS[0]);
const int GEAR_X = 452, GEAR_Y = 26;    // gear icon centre (tap area 64x64)

// ---------------- I2C: expander, touch, RTC ----------------
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

uint8_t expOut = 0x7F;                  // all high except P7 (speaker amp off)
void lcdHardReset() {
  i2cWrite(EXP_ADDR, 1, expOut);
  i2cWrite(EXP_ADDR, 3, 0b01110100);    // P0,P1,P3,P7 outputs
  expOut &= ~(1 << 1); i2cWrite(EXP_ADDR, 1, expOut); delay(20);
  expOut |=  (1 << 1); i2cWrite(EXP_ADDR, 1, expOut); delay(120);
}

// first finger, in landscape screen coordinates (false = no touch).
// The touch chip reports portrait coordinates; rotation 1 maps them like
// LovyanGFX does: x = raw y, y = 319 - raw x.
bool readTouch(int& x, int& y) {
  uint8_t d[5];
  if (i2cRead(TOUCH_ADDR, 0x02, d, 5) != 5) return false;
  int n = d[0] & 0x0F;
  if (n < 1 || n > 2) return false;
  int rx = ((d[1] & 0x0F) << 8) | d[2];
  int ry = ((d[3] & 0x0F) << 8) | d[4];
  x = ry;
  y = 319 - rx;
  return true;
}

// ---------------- audio: ES8311 codec + I2S, amp on expander P7 ----------------
I2SClass i2s;
const uint8_t CODEC_ADDR = 0x18;
const int SAMPLE_RATE = 16000;          // MCLK = 256 x 16 kHz = 4.096 MHz
bool audioOk = false;

uint8_t codecRead(uint8_t reg) {
  uint8_t v = 0;
  i2cRead(CODEC_ADDR, reg, &v, 1);
  return v;
}

void ampOn(bool on) {                   // speaker amplifier (PA_CTRL)
  if (on) expOut |= (1 << 7); else expOut &= ~(1 << 7);
  i2cWrite(EXP_ADDR, 1, expOut);
}

// ES8311 set-up for playback: 16-bit I2S, 16 kHz, MCLK from the MCLK pin
// (same register sequence as the ES8311 Arduino driver Waveshare ships)
bool codecInit(int volume) {
  Wire.beginTransmission(CODEC_ADDR);
  if (Wire.endTransmission() != 0) return false;
  i2cWrite(CODEC_ADDR, 0x00, 0x1F); delay(20);     // reset
  i2cWrite(CODEC_ADDR, 0x00, 0x00);
  i2cWrite(CODEC_ADDR, 0x00, 0x80);                // power on
  i2cWrite(CODEC_ADDR, 0x01, 0x3F);                // all clocks on
  i2cWrite(CODEC_ADDR, 0x06, codecRead(0x06) & ~(1 << 5));
  // clock dividers for MCLK 4.096 MHz / 16 kHz: pre_div 1, x1, adc/dac div 1,
  // lrck 0x00FF, bclk div 4, osr 0x10
  i2cWrite(CODEC_ADDR, 0x02, codecRead(0x02) & 0x07);
  i2cWrite(CODEC_ADDR, 0x03, 0x10);
  i2cWrite(CODEC_ADDR, 0x04, 0x10);
  i2cWrite(CODEC_ADDR, 0x05, 0x00);
  i2cWrite(CODEC_ADDR, 0x06, (codecRead(0x06) & 0xE0) | (4 - 1));
  i2cWrite(CODEC_ADDR, 0x07, codecRead(0x07) & 0xC0);
  i2cWrite(CODEC_ADDR, 0x08, 0xFF);
  i2cWrite(CODEC_ADDR, 0x09, codecRead(0x09) | (3 << 2));   // 16-bit in
  i2cWrite(CODEC_ADDR, 0x0A, codecRead(0x0A) | (3 << 2));   // 16-bit out
  i2cWrite(CODEC_ADDR, 0x0D, 0x01);                // analog power
  i2cWrite(CODEC_ADDR, 0x0E, 0x02);
  i2cWrite(CODEC_ADDR, 0x12, 0x00);                // DAC on
  i2cWrite(CODEC_ADDR, 0x13, 0x10);                // output driver on
  i2cWrite(CODEC_ADDR, 0x1C, 0x6A);
  i2cWrite(CODEC_ADDR, 0x37, 0x08);
  i2cWrite(CODEC_ADDR, 0x32, volume * 256 / 100 - 1);       // DAC volume
  return true;
}

// 10 ms of a 2 kHz tone and of silence, stereo 16-bit
int16_t toneBuf[SAMPLE_RATE / 100 * 2];
int16_t silenceBuf[SAMPLE_RATE / 100 * 2];

void playMs(const int16_t* buf, int ms) {
  for (int i = 0; i < ms / 10 && alarmRinging; i++)
    i2s.write((const uint8_t*)buf, sizeof(toneBuf));
}

// beep pattern while alarmRinging: 4 short beeps, a pause, repeat
void beepTask(void*) {
  for (;;) {
    if (!alarmRinging) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }
    while (alarmRinging) {
      for (int b = 0; b < 4 && alarmRinging; b++) { playMs(toneBuf, 100); playMs(silenceBuf, 80); }
      playMs(silenceBuf, 500);
    }
    for (int i = 0; i < 4; i++) i2s.write((const uint8_t*)silenceBuf, sizeof(silenceBuf));
  }
}

void audioInit() {
  for (int i = 0; i < SAMPLE_RATE / 100; i++) {
    int16_t v = (int16_t)(9000 * sinf(2 * PI * 2000.0f * i / SAMPLE_RATE));
    toneBuf[2 * i] = toneBuf[2 * i + 1] = v;
    silenceBuf[2 * i] = silenceBuf[2 * i + 1] = 0;
  }
  if (!codecInit(80)) { Serial.println("Audio codec not found - no beeps"); return; }
  i2s.setPins(13 /*BCLK*/, 15 /*LRCK*/, 16 /*DOUT -> codec*/, 14 /*DIN*/, 12 /*MCLK*/);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S start failed - no beeps");
    return;
  }
  audioOk = true;
  xTaskCreatePinnedToCore(beepTask, "beep", 4096, nullptr, 2, nullptr, 0);
}

void startAlarm(unsigned long limitMs) {
  alarmLimitMs = limitMs;
  alarmStartMs = millis();
  ampOn(true);                           // amp switched from here (I2C stays on one task)
  alarmRinging = true;
  Serial.println(audioOk ? "Alarm!" : "Alarm! (no audio - codec/I2S not running)");
}

void stopAlarm() {
  if (!alarmRinging) return;
  alarmRinging = false;
  delay(30);                             // let the last buffer finish
  ampOn(false);
  Serial.println("Alarm stopped");
}

uint8_t bcd2dec(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
uint8_t dec2bcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

// days since 1970-01-01 for a UTC date (no time zone involved)
long daysFromCivil(int y, int m, int d) {
  y -= m <= 2;
  long era = (y >= 0 ? y : y - 399) / 400;
  unsigned yoe = (unsigned)(y - era * 400);
  unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + (long)doe - 719468;
}

// RTC holds UTC. Returns 0 if the RTC lost power (or isn't there).
time_t rtcRead() {
  uint8_t r[7];
  if (i2cRead(RTC_ADDR, 0x04, r, 7) != 7) return 0;
  if (r[0] & 0x80) return 0;            // OS flag: oscillator stopped, time invalid
  int s = bcd2dec(r[0] & 0x7F), mi = bcd2dec(r[1] & 0x7F), h = bcd2dec(r[2] & 0x3F);
  int d = bcd2dec(r[3] & 0x3F), mo = bcd2dec(r[5] & 0x1F), y = 2000 + bcd2dec(r[6]);
  if (y < 2024 || mo < 1 || mo > 12 || d < 1) return 0;
  return (time_t)(daysFromCivil(y, mo, d) * 86400L + h * 3600L + mi * 60L + s);
}

void rtcWrite(time_t t) {
  struct tm u;
  gmtime_r(&t, &u);
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0x04);
  Wire.write(dec2bcd(u.tm_sec));        // also clears the OS flag
  Wire.write(dec2bcd(u.tm_min));
  Wire.write(dec2bcd(u.tm_hour));
  Wire.write(dec2bcd(u.tm_mday));
  Wire.write(u.tm_wday);
  Wire.write(dec2bcd(u.tm_mon + 1));
  Wire.write(dec2bcd(u.tm_year - 100));
  Wire.endTransmission();
}

// ---------------- time helpers ----------------
bool timeValid() { return time(nullptr) > 1700000000; }

bool nowLocal(struct tm& t) {
  if (!timeValid()) return false;
  time_t n = time(nullptr) + (demoActive ? demoOffset : 0);
  localtime_r(&n, &t);
  return true;
}

String fmtTime(time_t t) {
  if (t == 0) return "never";
  struct tm lt; localtime_r(&t, &lt);
  char b[32]; strftime(b, sizeof(b), "%a %b %d %H:%M", &lt);
  return String(b);
}

// ---------------- Wi-Fi sync ----------------
void onSntpSync(struct timeval*) { sntpDone = true; }

void wifiOff() {
  esp_sntp_stop();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiSyncing = false;
  ntpStarted = false;
}

void startSync() {
  if (wifiSsid.length() == 0) {
    Serial.println("No Wi-Fi set. Send: ssid <name>, then pass <password>");
    syncStart = millis();               // (keeps the 5-min retry from repeating every loop)
    return;
  }
  if (wifiSyncing) return;
  Serial.println("Sync: connecting to '" + wifiSsid + "'...");
  sntpDone = false;
  ntpStarted = false;
  wifiSyncing = true;
  syncStart = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
}

void serviceSync() {
  if (wifiSyncing) {
    if (!ntpStarted && WiFi.status() == WL_CONNECTED) {
      Serial.printf("Wi-Fi connected (%d dBm), getting time...\n", WiFi.RSSI());
      configTzTime(TZ_INFO, NTP1, NTP2);
      ntpStarted = true;
    }
    if (sntpDone) {
      lastSyncOk = time(nullptr);
      lastSyncFailed = false;
      if (rtcFound) rtcWrite(lastSyncOk);
      wifiOff();
      Serial.println("Synced: " + fmtTime(lastSyncOk) + " - RTC updated, Wi-Fi off");
    } else if (millis() - syncStart >= SYNC_TIMEOUT_MS) {
      lastSyncFailed = true;
      wifiOff();
      Serial.println(WiFi.status() == WL_CONNECTED ? "Sync failed: no answer from NTP"
                                                   : "Sync failed: couldn't join Wi-Fi - will retry");
    }
    return;
  }
  // schedule: daily at SYNC_HOUR:SYNC_MIN, hourly at :17 after a failure,
  // every 5 min while there's no valid time at all
  if (!timeValid()) {
    if (lastSyncFailed && millis() - syncStart >= 300000UL) startSync();
    return;
  }
  time_t n = time(nullptr);
  struct tm t; localtime_r(&n, &t);
  if (t.tm_min == SYNC_MIN && t.tm_hour != lastAttemptHour) {
    bool daily = t.tm_hour == SYNC_HOUR && t.tm_yday != lastSyncDay;
    if (daily || lastSyncFailed) {
      lastAttemptHour = t.tm_hour;
      if (daily) lastSyncDay = t.tm_yday;
      startSync();
    }
  }
}

// ---------------- drawing (all into the off-screen frame) ----------------
void polar(float deg, float r, int& x, int& y) {
  float a = deg * (PI / 180.0f);
  x = PX + (int)lroundf(r * sinf(a));
  y = PY - (int)lroundf(r * cosf(a));
}

float minuteToDeg(float m) { return -SPAN + m * (2 * SPAN / 60.0f); }

int hourShown(int h) {
  if (!hour12) return h;
  int v = h % 12;
  return v == 0 ? 12 : v;
}

void drawFan() {
  // thin arc outside the ticks (LovyanGFX angles: 0 = 3 o'clock, clockwise)
  frame.drawArc(PX, PY, R_ARC + 1, R_ARC, 270 - SPAN, 270 + SPAN, C_ARC);
  for (int m = 0; m <= 60; m++) {
    bool five = (m % 5 == 0);
    int x1, y1, x2, y2;
    float deg = minuteToDeg(m);
    polar(deg, five ? R_T5 : R_T1, x1, y1);
    polar(deg, R_TOUT, x2, y2);
    if (five) frame.drawWideLine(x1, y1, x2, y2, 1.5f, C_TICK5);   // 3 px wide
    else      frame.drawLine(x1, y1, x2, y2, C_TICK);
  }
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_center);
  const char* lab[5] = { "00", "15", "30", "45", "60" };
  for (int i = 0; i < 5; i++) {
    int x, y;
    polar(minuteToDeg(i * 15), R_LABEL, x, y);
    frame.drawString(lab[i], x, y);
  }
}

void drawSeconds(int sec) {             // sec = -1: track empty
  for (int i = 0; i < 60; i++) {
    float deg = minuteToDeg(i + 0.5f);
    float half = (2 * SPAN / 60.0f) * 0.32f;
    frame.fillArc(PX, PY, R_S_OUT, R_S_IN, deg + 270 - half, deg + 270 + half,
                  i <= sec ? C_SECON : C_SECOFF);
  }
}

void drawHand(float deg) {
  int tx, ty;
  polar(deg, R_HAND, tx, ty);
  float a = deg * (PI / 180.0f);
  float bx = PX + R_HAND0 * sinf(a), by = PY - R_HAND0 * cosf(a);
  float nx = cosf(a) * W_HAND / 2, ny = sinf(a) * W_HAND / 2;
  frame.fillTriangle(tx, ty, (int)(bx - nx), (int)(by - ny), (int)(bx + nx), (int)(by + ny), C_HAND);
}

// hour window; 'roll' 0..1 slides 'from' up and 'to' in from below
void drawHourWindow(int from, int to, float roll, bool known) {
  frame.drawRoundRect(WIN_X - 5, WIN_Y - 5, WIN_W + 10, WIN_H + 10, 10, C_WINRIM);
  frame.fillRect(WIN_X, WIN_Y, WIN_W, WIN_H, C_BG);
  frame.setClipRect(WIN_X, WIN_Y, WIN_W, WIN_H);
  frame.setFont(&fonts::DejaVu72);
  frame.setTextColor(C_HOUR);
  frame.setTextDatum(middle_center);
  int cx = WIN_X + WIN_W / 2, cy = WIN_Y + WIN_H / 2 + 2;
  char b[4];
  if (!known) {
    frame.drawString("--", cx, cy);
  } else {
    int off = (int)(roll * WIN_H);
    snprintf(b, sizeof(b), hour12 ? "%d" : "%02d", hourShown(from));
    frame.drawString(b, cx, cy - off);
    if (roll > 0) {
      snprintf(b, sizeof(b), hour12 ? "%d" : "%02d", hourShown(to));
      frame.drawString(b, cx, cy - off + WIN_H);
    }
  }
  frame.clearClipRect();
}

const char* WD[7] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
const char* MO[12] = { "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC" };

void drawDate(const struct tm& t, bool known) {
  frame.setTextColor(C_DATE);
  frame.setFont(&fonts::DejaVu40);
  frame.setTextDatum(bottom_left);
  frame.drawString(known ? WD[t.tm_wday] : "---", 22, H - 18);
  frame.setTextDatum(bottom_right);
  char b[4];
  snprintf(b, sizeof(b), "%02d", t.tm_mday);
  frame.drawString(known ? b : "--", W - 22, H - 18);
  frame.setFont(&fonts::DejaVu24);
  frame.drawString(known ? MO[t.tm_mon] : "---", W - 22, H - 66);
}

// little Wi-Fi state dot, top-left: blue while syncing, red after a failure
void drawStatusDot() {
  if (wifiSyncing) frame.fillCircle(14, 14, 5, C_DATE);
  else if (lastSyncFailed || wifiSsid.length() == 0) frame.fillCircle(14, 14, 5, C_BAD);
}

void drawGear(int cx, int cy, uint16_t c) {
  for (int i = 0; i < 8; i++) {          // teeth
    float a = i * PI / 4;
    frame.fillCircle(cx + (int)(12 * cosf(a)), cy + (int)(12 * sinf(a)), 4, c);
  }
  frame.fillCircle(cx, cy, 11, c);
  frame.fillCircle(cx, cy, 5, C_BG);
}

void drawBell(int x, int y, uint16_t c) { // small bell, top-left at x,y (16x18)
  frame.fillCircle(x + 8, y + 7, 6, c);
  frame.fillRect(x + 2, y + 7, 13, 6, c);
  frame.fillRect(x, y + 12, 17, 3, c);
  frame.fillCircle(x + 8, y + 17, 2, c);
}

void drawAlarmIndicator() {
  if (!alarmOn) return;
  drawBell(30, 6, C_HOUR);
  char b[8];
  snprintf(b, sizeof(b), "%02d:%02d", alarmH, alarmM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_HOUR);
  frame.setTextDatum(top_left);
  frame.drawString(b, 52, 6);
}

void drawRinging() {                     // flashing banner while the alarm beeps
  if ((millis() / 400) % 2) return;
  frame.fillRoundRect(90, 110, 300, 90, 16, C_HOUR);
  frame.setFont(&fonts::DejaVu40);
  frame.setTextColor(TFT_BLACK);
  frame.setTextDatum(middle_center);
  frame.drawString("ALARM", 240, 142);
  frame.setFont(&fonts::DejaVu18);
  frame.drawString("touch to stop", 240, 182);
}

void drawCard() {
  frame.fillRoundRect(60, 60, 360, 190, 14, C_CARD);
  frame.drawRoundRect(60, 60, 360, 190, 14, C_LABEL);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(top_left);
  int y = 76;
  auto line = [&](const String& s) { frame.drawString(s, 80, y); y += 30; };
  line("Wi-Fi: " + (wifiSsid.length() ? wifiSsid : String("not set")));
  line(String("Status: ") + (wifiSyncing ? "syncing..." : (lastSyncFailed ? "last sync FAILED" : "off (saves power)")));
  line("Last sync: " + fmtTime(lastSyncOk));
  char b[40]; snprintf(b, sizeof(b), "Next sync: %d:%02d AM daily", SYNC_HOUR, SYNC_MIN);
  line(b);
  line(String("RTC: ") + (rtcFound ? (rtcValidAtBoot ? "OK" : "was reset") : "not found"));
}

// one complete frame
void render(float handDeg, int sec, int hFrom, int hTo, float roll, bool known, const struct tm& t) {
  frame.fillScreen(C_BG);
  drawFan();
  drawSeconds(known ? sec : -1);
  drawHand(handDeg);
  drawHourWindow(hFrom, hTo, roll, known);
  drawDate(t, known);
  drawStatusDot();
  drawAlarmIndicator();
  drawGear(GEAR_X, GEAR_Y, C_LABEL);
  if (millis() < cardUntil) drawCard();
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// ---------------- settings screen ----------------
const Btn* findBtn(int id) {
  for (int i = 0; i < NBTN; i++) if (BTNS[i].id == id) return &BTNS[i];
  return nullptr;
}

int hitTest(int x, int y) {
  for (int i = 0; i < NBTN; i++) {
    const Btn& b = BTNS[i];
    if (x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h) return b.id;
  }
  return B_NONE;
}

void drawArrow(int id, bool up, bool pressed) {
  const Btn* b = findBtn(id);
  uint16_t c = pressed ? C_HOUR : C_LABEL;
  int cx = b->x + b->w / 2, cy = b->y + b->h / 2;
  if (up) frame.fillTriangle(cx - 16, cy + 9, cx + 16, cy + 9, cx, cy - 11, c);
  else    frame.fillTriangle(cx - 16, cy - 9, cx + 16, cy - 9, cx, cy + 11, c);
}

void drawButton(int id, const char* label, uint16_t fill, uint16_t text) {
  const Btn* b = findBtn(id);
  frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, fill);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(text);
  frame.setTextDatum(middle_center);
  frame.drawString(label, b->x + b->w / 2, b->y + b->h / 2 + 1);
}

void drawTimeFields(int xH, int xM, int h, int m, uint16_t c) {
  char b[4];
  frame.setFont(&fonts::DejaVu56);
  frame.setTextColor(c);
  frame.setTextDatum(middle_center);
  snprintf(b, sizeof(b), "%02d", h); frame.drawString(b, xH, 170);
  frame.drawString(":", (xH + xM) / 2, 166);
  snprintf(b, sizeof(b), "%02d", m); frame.drawString(b, xM, 170);
}

int pressedBtn = B_NONE;

void renderSettings() {
  frame.fillScreen(C_BG);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_left);
  frame.drawString("Settings", 16, 30);
  drawButton(B_DONE, "DONE", C_DATE, TFT_BLACK);

  // left: set the time
  frame.drawRoundRect(15, 66, 210, 192, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_center);
  frame.drawString("TIME", 120, 80);
  drawArrow(B_TH_UP, true, pressedBtn == B_TH_UP);  drawArrow(B_TH_DN, false, pressedBtn == B_TH_DN);
  drawArrow(B_TM_UP, true, pressedBtn == B_TM_UP);  drawArrow(B_TM_DN, false, pressedBtn == B_TM_DN);
  drawTimeFields(70, 170, editH, editM, TFT_WHITE);
  drawButton(B_SET, "SET TIME", 0x4208, TFT_WHITE);

  // right: alarm
  frame.drawRoundRect(255, 66, 210, 192, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_center);
  frame.drawString("ALARM", 360, 80);
  drawArrow(B_AH_UP, true, pressedBtn == B_AH_UP);  drawArrow(B_AH_DN, false, pressedBtn == B_AH_DN);
  drawArrow(B_AM_UP, true, pressedBtn == B_AM_UP);  drawArrow(B_AM_DN, false, pressedBtn == B_AM_DN);
  drawTimeFields(310, 410, alarmH, alarmM, alarmOn ? C_HOUR : 0x8410);
  drawButton(B_AON, alarmOn ? "ON" : "OFF", alarmOn ? C_OK : 0x4208, alarmOn ? TFT_BLACK : TFT_WHITE);
  drawButton(B_TEST, "TEST", 0x4208, TFT_WHITE);

  if (millis() < savedMsgUntil) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(C_OK);
    frame.setTextDatum(middle_center);
    frame.drawString(savedMsg, 240, 30);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

void openSettings() {
  struct tm t;
  if (nowLocal(t)) { editH = t.tm_hour; editM = t.tm_min; }
  screen = SCREEN_SETTINGS;
  lastTouchMs = millis();
  renderSettings();
}

void closeSettings() {
  prefs.putInt("alH", alarmH);
  prefs.putInt("alM", alarmM);
  prefs.putBool("alOn", alarmOn);
  screen = SCREEN_FACE;
  shownHour = -1;                         // no hour animation for a jump
  updateClock(true);
}

void setClockTo(int h, int m) {
  struct tm t;
  if (!nowLocal(t)) {                     // no date yet: assume 2026-01-01
    memset(&t, 0, sizeof(t));
    t.tm_year = 126; t.tm_mon = 0; t.tm_mday = 1;
  }
  t.tm_hour = h; t.tm_min = m; t.tm_sec = 0; t.tm_isdst = -1;
  time_t nt = mktime(&t);                 // local (Pacific) -> UTC
  struct timeval tv = { nt, 0 };
  settimeofday(&tv, nullptr);
  if (rtcFound) rtcWrite(nt);
  Serial.printf("Time set to %02d:%02d (the next Wi-Fi sync will correct it)\n", h, m);
}

// one press (or one auto-repeat step) of a settings button
void pressButton(int id) {
  switch (id) {
    case B_TH_UP: editH = (editH + 1) % 24; break;
    case B_TH_DN: editH = (editH + 23) % 24; break;
    case B_TM_UP: editM = (editM + 1) % 60; break;
    case B_TM_DN: editM = (editM + 59) % 60; break;
    case B_AH_UP: alarmH = (alarmH + 1) % 24; break;
    case B_AH_DN: alarmH = (alarmH + 23) % 24; break;
    case B_AM_UP: alarmM = (alarmM + 1) % 60; break;
    case B_AM_DN: alarmM = (alarmM + 59) % 60; break;
    case B_SET:
      setClockTo(editH, editM);
      savedMsg = "Time set"; savedMsgUntil = millis() + 1500;
      break;
    case B_AON:
      alarmOn = !alarmOn;
      prefs.putBool("alOn", alarmOn);
      savedMsg = alarmOn ? "Alarm on" : "Alarm off"; savedMsgUntil = millis() + 1500;
      break;
    case B_TEST:
      if (alarmRinging) stopAlarm(); else startAlarm(3000);
      break;
    case B_DONE:
      closeSettings();
      return;
  }
  renderSettings();
}

bool isArrow(int id) {
  return id == B_TH_UP || id == B_TH_DN || id == B_TM_UP || id == B_TM_DN ||
         id == B_AH_UP || id == B_AH_DN || id == B_AM_UP || id == B_AM_DN;
}

// ---------------- the clock ----------------

float handDegFor(const struct tm& t) { return minuteToDeg(t.tm_min + t.tm_sec / 60.0f); }

// top of the hour: hand flies back with an ease-out, then the digits roll
void hourChange(const struct tm& t, int oldHour) {
  float from = minuteToDeg(60), to = minuteToDeg(0);
  for (int f = 1; f <= 18; f++) {
    float u = f / 18.0f;
    float e = 1 - (1 - u) * (1 - u) * (1 - u);
    render(from + (to - from) * e, -1, oldHour, oldHour, 0, true, t);
  }
  for (int f = 1; f <= 14; f++) {
    float u = f / 14.0f;
    float e = u * u * (3 - 2 * u);      // smoothstep
    render(to, -1, oldHour, t.tm_hour, e, true, t);
  }
}

void updateClock(bool force) {
  struct tm t = {};
  bool known = nowLocal(t);
  if (!known) {
    if (force || shownSec != -2) { render(minuteToDeg(0), -1, 0, 0, 0, false, t); shownSec = -2; }
    return;
  }
  if (!force && t.tm_sec == shownSec) return;
  if (shownHour >= 0 && t.tm_hour != shownHour && t.tm_min == 0 && !force) hourChange(t, shownHour);
  shownHour = t.tm_hour;
  shownSec = t.tm_sec;
  shownHand = handDegFor(t);
  render(shownHand, t.tm_sec, t.tm_hour, t.tm_hour, 0, true, t);
}

// ---------------- commands ----------------
void handleCommand(String in) {
  in.trim();
  String lower = in; lower.toLowerCase();
  if (lower.startsWith("ssid ")) {
    wifiSsid = in.substring(5); wifiSsid.trim();
    prefs.putString("ssid", wifiSsid);
    Serial.println("Wi-Fi name saved: " + wifiSsid + " - now send: pass <password>");
  } else if (lower.startsWith("pass ")) {
    wifiPass = in.substring(5);
    prefs.putString("pass", wifiPass);
    Serial.printf("Password saved (%d characters). Syncing...\n", wifiPass.length());
    startSync();
  } else if (lower == "sync") {
    startSync();
  } else if (lower == "status") {
    struct tm t;
    if (nowLocal(t)) { char b[40]; strftime(b, sizeof(b), "%a %Y-%m-%d %H:%M:%S %Z", &t); Serial.println(String("Time: ") + b); }
    else Serial.println("Time: not set yet");
    Serial.println("Wi-Fi: " + (wifiSsid.length() ? wifiSsid : String("not set")) + (wifiSyncing ? " (syncing)" : " (off)"));
    Serial.println("Last sync: " + fmtTime(lastSyncOk) + (lastSyncFailed ? "  - last attempt FAILED" : ""));
    Serial.printf("Daily sync: %d:%02d\n", SYNC_HOUR, SYNC_MIN);
    Serial.println(String("RTC: ") + (rtcFound ? (rtcValidAtBoot ? "found, time was valid at boot" : "found, had lost its time") : "not found"));
  } else if (lower == "12h" || lower == "24h") {
    hour12 = (lower == "12h");
    prefs.putBool("h12", hour12);
    updateClock(true);
    Serial.println(hour12 ? "12-hour" : "24-hour");
  } else if (lower.startsWith("bl")) {
    backlight = constrain(lower.substring(2).toInt(), 0, 255);
    lcd.setBrightness(backlight);
    prefs.putInt("bl", backlight);
    Serial.printf("backlight %d\n", backlight);
  } else if (lower.startsWith("alarm")) {
    String a = lower.substring(5); a.trim();
    int h, m;
    if (a == "on" || a == "off") alarmOn = (a == "on");
    else if (sscanf(a.c_str(), "%d:%d", &h, &m) == 2 && h >= 0 && h < 24 && m >= 0 && m < 60) { alarmH = h; alarmM = m; alarmOn = true; }
    else if (a == "test") { startAlarm(3000); return; }
    else { Serial.println("alarm HH:MM | alarm on | alarm off | alarm test"); return; }
    prefs.putInt("alH", alarmH); prefs.putInt("alM", alarmM); prefs.putBool("alOn", alarmOn);
    Serial.printf("Alarm %02d:%02d %s\n", alarmH, alarmM, alarmOn ? "ON" : "OFF");
    updateClock(true);
  } else if (lower == "demo") {
    struct tm t;
    if (!nowLocal(t)) { Serial.println("No time yet"); return; }
    demoOffset = (59 - t.tm_min) * 60L + (55 - t.tm_sec);   // land on hh:59:55
    demoActive = true;
    demoStart = millis();
    updateClock(true);
    Serial.println("demo: hour change in 5 s, real time back after 8 s");
  } else {
    Serial.println("ssid <name> | pass <password> | sync | status | 12h | 24h | bl <0-255> | alarm HH:MM|on|off|test | demo");
  }
}

// ---------------- setup / loop ----------------
void setup() {
  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 1500);

  prefs.begin("jumphour", false);
  wifiSsid  = prefs.getString("ssid", "");
  wifiPass  = prefs.getString("pass", "");
  hour12    = prefs.getBool("h12", false);
  backlight = prefs.getInt("bl", 220);
  alarmH    = prefs.getInt("alH", 7);
  alarmM    = prefs.getInt("alM", 0);
  alarmOn   = prefs.getBool("alOn", false);

  Wire.begin(PIN_SDA, PIN_SCL, 400000);
  lcdHardReset();
  lcd.init();
  lcd.setRotation(1);                   // landscape 480 x 320
  lcd.setBrightness(backlight);
  frame.setPsram(true);
  frame.setColorDepth(16);
  if (!frame.createSprite(W, H)) Serial.println("PSRAM frame FAILED - set PSRAM: OPI PSRAM");

  // time zone first, then the RTC gives the time right away
  setenv("TZ", TZ_INFO, 1);
  tzset();
  Wire.beginTransmission(RTC_ADDR);
  rtcFound = (Wire.endTransmission() == 0);
  time_t r = rtcFound ? rtcRead() : 0;
  if (r) {
    struct timeval tv = { r, 0 };
    settimeofday(&tv, nullptr);
    rtcValidAtBoot = true;
    Serial.println("Time from RTC: " + fmtTime(r));
  } else {
    Serial.println(rtcFound ? "RTC had lost its time - waiting for Wi-Fi" : "No RTC found");
  }

  audioInit();
  sntp_set_time_sync_notification_cb(onSntpSync);
  WiFi.mode(WIFI_OFF);
  updateClock(true);
  startSync();                          // sync at power-up
  if (wifiSsid.length() == 0) lastSyncFailed = true;
}

void loop() {
  static String buf;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { if (buf.length()) handleCommand(buf); buf = ""; }
    else if (buf.length() < 120) buf += c;
  }

  serviceSync();

  if (demoActive && millis() - demoStart >= 8000) {
    demoActive = false;
    demoOffset = 0;
    shownHour = -1;
    updateClock(true);
    Serial.println("demo over - real time");
  }

  // ---- alarm: goes off once at alarmH:alarmM, rings 15 s ----
  struct tm now;
  if (alarmOn && !demoActive && nowLocal(now) && now.tm_hour == alarmH && now.tm_min == alarmM) {
    long key = now.tm_yday * 1440L + alarmH * 60 + alarmM;
    if (key != lastAlarmKey) { lastAlarmKey = key; startAlarm(15000); }
  }
  if (alarmRinging && millis() - alarmStartMs >= alarmLimitMs) stopAlarm();

  // ---- touch ----
  static bool wasTouched = false;
  static unsigned long pressStart = 0, lastRepeat = 0;
  int tx = 0, ty = 0;
  bool tnow = readTouch(tx, ty);
  bool down = tnow && !wasTouched;
  wasTouched = tnow;
  if (tnow) lastTouchMs = millis();

  if (down && alarmRinging) {
    stopAlarm();                          // any touch stops the alarm
    pressedBtn = B_NONE;
    if (screen == SCREEN_SETTINGS) renderSettings(); else updateClock(true);
  } else if (screen == SCREEN_FACE) {
    if (down) {
      if (abs(tx - GEAR_X) < 32 && abs(ty - GEAR_Y) < 32) openSettings();   // only the gear reacts
    }
  } else {                                // settings
    if (down) {
      pressedBtn = hitTest(tx, ty);
      pressStart = lastRepeat = millis();
      if (pressedBtn != B_NONE) pressButton(pressedBtn);
    } else if (tnow && isArrow(pressedBtn) && millis() - pressStart > 450 && millis() - lastRepeat > 110) {
      lastRepeat = millis();                // hold an arrow: it repeats
      pressButton(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) {
      pressedBtn = B_NONE;
      if (screen == SCREEN_SETTINGS) renderSettings();
    }
    if (screen == SCREEN_SETTINGS && millis() - lastTouchMs > 60000) closeSettings();
  }

  if (screen == SCREEN_FACE) {
    static bool cardShown = false;
    bool cardNow = millis() < cardUntil;
    if (cardShown && !cardNow) updateClock(true);   // card just expired
    cardShown = cardNow;
    static bool wasRinging = false;
    if (alarmRinging || wasRinging) {     // keep the banner flashing
      static unsigned long lastFlash = 0;
      if (millis() - lastFlash >= 200) { lastFlash = millis(); updateClock(true); }
    }
    wasRinging = alarmRinging;
    updateClock(false);
  } else {
    static unsigned long lastRedraw = 0;  // settings: refresh for notes / banner
    if ((alarmRinging || millis() < savedMsgUntil + 300) && millis() - lastRedraw > 200) {
      lastRedraw = millis();
      renderSettings();
    }
  }
  delay(15);
}
