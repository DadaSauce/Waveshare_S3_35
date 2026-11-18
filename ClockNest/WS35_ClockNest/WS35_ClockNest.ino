/*
  =============================================================================
   Clock + Nest home hub - Waveshare ESP32-S3-Touch-LCD-3.5 (320 x 480)
  =============================================================================
  THE FACE (upright)
    top half   the clock, always 24-hour - three faces, chosen on
               Settings > Clock (FACE) or with the "face" command:
               JUMP (default) jump-hour retrograde clock: the minute hand
                 sweeps a 116 deg fan 00 -> 60 and snaps back at the top of
                 the hour while the amber hour digits roll; cyan seconds track.
               ANALOG the round Desk Pet's analog look: white hour ticks,
                 grey minute ticks, bold 12 / 3 / 6 / 9, amber hour, white
                 minute and cyan seconds hands, cyan centre cap.
               DIGITAL the Kindle Home Hub's style (clock_fonts.h): big amber
                 HH:MM with cyan seconds, weekday in white, full date in sky
                 blue. It shows its own date, so the date corners stay empty
                 and all status icons + the bell sit in one row on top.
    corners    top left: Wi-Fi and Bluetooth status icons
                 (grey = off, lit = on and connected, red cross = on but not
                 connected / no paired phone), Nest under Wi-Fi (flame =
                 heating, snowflake = cooling, grey = idle / off); analog
                 face: one column - Wi-Fi, Bluetooth, Nest, chime
               chime note (amber = hourly chime on, hollow grey = quiet
                 hours now, none = chime off): jump hour = next to the alarm
                 bell over the weekday, analog = under the Nest, digital =
                 after the bell in the top row, sideways = under Bluetooth
               top right: battery - fill shows the charge (red < 10%,
                 yellow < 35%, green), lightning bolt while charging
               top center: a small handle - swipe down here for settings
               bottom left: weekday, with the alarm bell above it
                 (amber = on, flashing = ringing, none = alarm off)
               bottom right: day, with the month above it
    bottom     weather (now + today and 2 days) or the Nest thermostat - tap
    half       the WEATHER / NEST pill to switch. The Nest panel goes back to
               weather after 15 s without a touch.
               NEST: room temperature, humidity, heating / cooling / idle,
               target with up / down (sent 1.5 s after the last tap), modes
               HEAT, COOL, AUTO, OFF, ECO, in the thermostat's own unit.

  TURN IT (motion sensor)
    upright / upside down   the face above (picture turned to read correctly)
    turned 90 deg left      the chosen clock face alone, large, with Wi-Fi, BT,
                            Nest, battery
    turned 90 deg right     full-screen weather: now + 7 days
    (Serial: orient | orient flip | orient swap | orient off | orient on)

  SETTINGS - swipe down from the top edge; the grid slides down over the
  clock. Swipe up (or DONE) to close; < BACK on a page returns to the grid.
  After 20 s without a touch, settings slide away back to the clock.
    Clock      FACE (jump / analog / digital), time + SET, Wi-Fi SYNC;
               alarm time, ON/OFF, VOLUME
               (three beeps at the new level), alarm sound (tap its name:
               an MP3 / AAC / FLAC / WAV / OGG on the SD card, or the beep)
    Display    brightness on battery (10% steps; always full on USB), sleep
               time on battery, keep Wi-Fi / Bluetooth on while asleep,
               wake on tap / move
    Weather    ZIP code
    Wi-Fi      on/off, scan, pick a network, on-screen keyboard
    Bluetooth  on/off, pairing PIN; a paired phone (BLE terminal app) can
               send the same commands as the Serial Monitor
    SD Card    info, browse, format (FAT32), USB DRIVE (card appears on the
               computer as a disk)
    Sound      hourly chime ON/OFF, chime volume (5% steps, separate from the
               alarm's), TEST, chime sound (tap: a file on the SD card, or
               the built-in two-note bell); QUIET HOURS ON/OFF, FROM / TO
               hour (no chime from FROM:00 until TO:00)
    Radio      placeholder (coming later)

  CHIME: on the hour (hh:00) unless it is in the quiet hours or the alarm
  rings that minute. Plays once; a tap doesn't stop it (it's short).

  ALARM: the bell and the backlight flash and it beeps for 1 minute, or plays the chosen
  file (looping) for up to 5 minutes. Touch anywhere to stop. If a settings
  page is open it returns to the clock face first.

  BATTERY: PWR held 6 s = off, one press = on. On battery the screen fades
  out after the sleep time; a touch - or a tap, move or shake (motion
  sensor, can be switched off in Display) - fades it back in. While dark
  the display controller sleeps and the processor drops to 80 MHz; the
  `power` command shows time asleep and what woke it.

  TIME: NTP at power-up and every 24 h (Wi-Fi stays connected for the Nest
  and weather); the RTC keeps time through power cuts. US Pacific + DST.

  NOTHING SECRET IN THIS SKETCH. Wi-Fi is entered on screen (or over USB /
  Bluetooth); the Nest sign-in is done once by Python/nest_auth.py on the Mac,
  which sends the credentials over USB. All are stored in the board's flash.

  LIBRARIES: "LovyanGFX" (lovyan03), "ArduinoJson" (Benoit Blanchon, v7),
             "NimBLE-Arduino" (h2zero, v2.x),
             "ESP32-audioI2S" (schreibfaul1) - the copy Waveshare ships,
             v3.3.0: install ESP32-audioI2S-master.zip from the project
             folder with Sketch > Include Library > Add .ZIP Library.
  BOARD:     "ESP32S3 Dev Module", USB CDC On Boot: Enabled, Flash 16MB,
             PSRAM: OPI PSRAM, Partition: 16M Flash (3MB APP/9.9MB FATFS).
             For USB DRIVE mode also: USB Mode: USB-OTG (TinyUSB) and
             Upload Mode: USB-OTG CDC (TinyUSB). Without those it still
             builds; the USB button just explains what to change.
  TABS:      google_roots.h, weather_roots.h (public root certificates
             for HTTPS - no secrets), clock_fonts.h (digital face fonts)

  COMMANDS - Serial Monitor (115200, "Newline") or a paired phone
    ssid <name> / pass <password>     Wi-Fi (saved)
    wifi on|off, bt on|off, bt        radios; bt = Bluetooth status + PIN
    sync                              NTP time sync now
    status                            time, Wi-Fi, sync, RTC, battery, Nest
    bl <10-100>                       battery brightness % (time is always 24-hour)
    face [jump|analog|digital]        clock face (no word = next face)
    alarm HH:MM | on | off | test     alarm;  snd = alarm sound status
    chime [on|off|test|vol <0-100>|quiet on|off|HH-HH]   hourly chime
    demo                              show the hour change now
    city <ZIP or name> / loc <lat>,<lon> / wx    weather place / status
    sd [path]                         card info + list a folder
    power                             time asleep, wake count by reason, battery
    charge [200-1000]                 battery charge current in mA (default 400)
    chargev [4.1|4.2]                 battery charge-to voltage (default 4.2)
    orient [flip|swap|off|on]         turning behaviour
    nest project|client|secret|token <value>   Nest credentials (saved)
    nest status | nest clear | nest poll
  =============================================================================
*/

/*
  =============================================================================
   HOW THIS SKETCH IS ORGANISED (read this first)
  =============================================================================
  TASKS - several things run at the same time on the ESP32-S3's two cores:
    loop()          core 1  the main program: touch, drawing, alarm, sleep,
                            battery, motion sensor, Serial / phone commands.
                            The only task that uses I2C and draws on screen.
    nestTask()      core 0  the network task: weather and Nest over HTTPS.
                            Slow requests live here so the screen never stalls.
    beepTask()      core 0  writes the beep pattern to I2S while ringing,
                            and the built-in chime bell.
    audio library   own     decodes the alarm MP3 (ESP32-audioI2S).
    NimBLE host     own     Bluetooth; its callbacks only set flags /
                            copy text - loop() acts on them.
    TinyUSB         own     USB serial and USB drive mode callbacks.
  Data shared between tasks (nest, wx, command requests) is copied under the
  'nestLock' mutex or kept in 'volatile' flags; nestVersion++ tells loop()
  that new data arrived and the screen should be redrawn.

  SCREENS - 'screen' says what is showing: the clock face, or one of the
  settings pages (grid, Clock, Display, Weather, Wi-Fi, keyboard, Bluetooth,
  SD card, USB drive, Sound). Each page has a renderXxx() that draws it, a list of
  buttons (Btn id/x/y/w/h) and a pressXxx() that handles a tap. loop() reads
  the touch chip, detects a new press ('down'), finds the button with
  hitTest() and calls the page's press function. Settings open with a swipe
  down from the top edge of the face (slideSettingsOpen) and close with a
  swipe up or DONE (slideSettingsClosed) - the grid slides like a shade.

  DRAWING - everything is drawn into 'frame', a full-screen sprite in PSRAM,
  then pushed to the display with DMA (about 30 ms per frame). No flicker.
  The face is redrawn once a second, or right away when something changes.
  The DIGITAL face uses anti-aliased VLW fonts (clock_fonts.h, loaded with
  frame.loadFont); their glyphs are unpacked on the stack, so loop() runs
  with a 16 KB stack (SET_LOOP_TASK_STACK_SIZE, next to setup()).

  ORIENTATION - 'orient' (0 upright, 1 sideways weather, 2 sideways clock,
  3 upside down) chooses the frame size, the display rotation and which
  render function draws the face.

  SAVED SETTINGS - Preferences (flash), namespace "clocknest". Nothing secret
  is in this file: Wi-Fi, Nest and Bluetooth secrets are typed in on the
  device and stored there.

  MAP OF THIS FILE (search for the capitalised section names):
    BOARD, COLORS, CLOCK GEOMETRY, SETTINGS / STATE, I2C, AUDIO, TIME HELPERS,
    NEST, WEATHER, WI-FI + NTP, DRAWING, BATTERY, ANALOG AND DIGITAL FACES,
    SIDEWAYS SCREENS, SETTINGS
    SCREEN, BLUETOOTH, SHARED PAGE WIDGETS, BLUETOOTH PAGE, WI-FI PAGE,
    ON-SCREEN KEYBOARD, SD CARD, WEATHER SETTINGS PAGE, ALARM MUSIC, USB DRIVE
    MODE, DISPLAY PAGE, SOUND PAGE, SLEEP / WAKE, NEST PANEL TOUCHES, THE CLOCK,
    ORIENTATION, COMMANDS, SETUP / LOOP.
  =============================================================================
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "ESP_I2S.h"
#include "esp_vfs_fat.h"                 // SD card (ESP-IDF SDMMC + FAT)
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <NimBLEDevice.h>
#include "Audio.h"                       // ESP32-audioI2S: MP3/AAC/FLAC/WAV decoder
#include "vfs_api.h"
#if ARDUINO_USB_MODE == 0                // USB-OTG (TinyUSB): USB drive mode available
#include "USB.h"
#include "USBMSC.h"
#define HAVE_USB_DRIVE 1
#else
#define HAVE_USB_DRIVE 0
#endif
#include <algorithm>
#include "google_roots.h"
#include "weather_roots.h"
#include "clock_fonts.h"            // smooth fonts for the DIGITAL face
using namespace lgfx::textdatum;        // middle_center, bottom_left, ...

// ============================================================================
// BOARD
// ----------------------------------------------------------------------------
// Pins and I2C addresses of the Waveshare ESP32-S3-Touch-LCD-3.5, and the
// LovyanGFX driver set-up for its ST7796 panel. The display is on SPI (80 MHz,
// DMA); its chip-select is tied low and its reset goes through the I/O
// expander. Backlight = PWM on GPIO 6.
// ============================================================================
const int PIN_SDA = 8, PIN_SCL = 7;
const uint8_t TOUCH_ADDR = 0x38;        // FT6336
const uint8_t EXP_ADDR   = 0x20;        // TCA9554 IO expander
const uint8_t RTC_ADDR   = 0x51;        // PCF85063
const uint8_t PMU_ADDR   = 0x34;        // AXP2101 power / battery charger
const uint8_t IMU_ADDR   = 0x6B;        // QMI8658 accelerometer / gyro

// LovyanGFX display driver for this board: ST7796 panel on SPI2 at 80 MHz
// with DMA, colors inverted (IPS panel), no CS/RST pins (handled elsewhere),
// backlight as PWM on GPIO 6.
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

// ---- types first (the Arduino IDE puts function declarations above the
// ---- first function, and they need these to exist)
struct Btn { int id, x, y, w, h; };       // a touch button: id + rectangle

struct NestState {                      // what the Nest task last read
  bool  valid = false;
  float ambientC = 0, heatC = 0, coolC = 0;
  int   humidity = -1;
  char  mode[12] = "";                  // HEAT COOL HEATCOOL OFF
  char  eco[12] = "";                   // OFF MANUAL_ECO
  char  hvac[12] = "";                  // HEATING COOLING OFF
  bool  scaleF = true;
  unsigned long updatedMs = 0;
  char  error[64] = "";
};

LGFX lcd;
LGFX_Sprite frame(&lcd);                // 320 x 480 off-screen frame in PSRAM
int W = 320, H = 480;                    // current frame size (480 x 320 when sideways)

// ============================================================================
// COLORS
// ----------------------------------------------------------------------------
// RGB565 colors (5 bits red, 6 green, 5 blue) used across all screens.
// ============================================================================
const uint16_t C_BG     = TFT_BLACK;
const uint16_t C_ARC    = 0x4208;
const uint16_t C_TICK   = 0x8410;
const uint16_t C_TICK5  = 0xFFFF;
const uint16_t C_LABEL  = 0xDEFB;
const uint16_t C_HAND   = 0xFFFF;
const uint16_t C_SECON  = 0x07FF;
const uint16_t C_SECOFF = 0x2104;
const uint16_t C_HOUR   = 0xFD80;       // amber
const uint16_t C_WINRIM = 0x8410;
const uint16_t C_DATE   = 0x2D7F;       // sky blue
const uint16_t C_OK = 0x07E0, C_BAD = 0xF800;
const uint16_t C_HEAT   = 0xFC60;       // orange
const uint16_t C_COOL   = 0x3D9F;       // blue
const uint16_t C_PANEL  = 0x18E3;       // dark button
const uint16_t C_DIM    = 0x8410;
const uint16_t C_RADIO_OFF = 0x3186;      // status icon that is off

// ============================================================================
// CLOCK GEOMETRY (TOP HALF)
// ----------------------------------------------------------------------------
// Every size and position of the jump-hour clock. They are variables, not
// constants, because turning the board sideways swaps in the big layout
// (setClockGeometry). Angles: 0 deg = straight up, the fan spans +-58 deg.
// ============================================================================
// clock geometry: portrait values here; setClockGeometry(true) switches to
// the big landscape clock (same design as WS35_JumpHour)
int   PX = 160, PY = 168;               // pivot, just inside the window top
const float SPAN = 58;
float R_ARC = 140, R_T5 = 120, R_T1 = 129, R_TOUT = 137, R_LABEL = 151;
float R_S_IN = 88, R_S_OUT = 99;
float R_HAND0 = 20, R_HAND = 113, W_HAND = 9;
int   WIN_W = 104, WIN_H = 66, WIN_X = PX - WIN_W / 2, WIN_Y = 165;
int   WIN_PAD = 4;
float TICK5_W = 1.2f;
const lgfx::IFont* labelFont = &fonts::DejaVu12;
const lgfx::IFont* hourFont = &fonts::DejaVu56;

// setClockGeometry(): Switches every clock dimension (pivot, radii, hour window, fonts) between the
//   portrait layout and the big sideways clock. Called on rotation; all the clock
//   drawing functions read these globals, so the same code draws both sizes.
void setClockGeometry(bool land) {
  if (land) {
    PX = 240; PY = 222;
    R_ARC = 196; R_T5 = 168; R_T1 = 180; R_TOUT = 192; R_LABEL = 207;
    R_S_IN = 126; R_S_OUT = 140; R_HAND0 = 26; R_HAND = 160; W_HAND = 12;
    WIN_W = 150; WIN_H = 96; WIN_Y = 218; WIN_PAD = 5; TICK5_W = 1.5f;
    labelFont = &fonts::DejaVu18; hourFont = &fonts::DejaVu72;
  } else {
    PX = 160; PY = 168;
    R_ARC = 140; R_T5 = 120; R_T1 = 129; R_TOUT = 137; R_LABEL = 151;
    R_S_IN = 88; R_S_OUT = 99; R_HAND0 = 20; R_HAND = 113; W_HAND = 9;
    WIN_W = 104; WIN_H = 66; WIN_Y = 165; WIN_PAD = 4; TICK5_W = 1.2f;
    labelFont = &fonts::DejaVu12; hourFont = &fonts::DejaVu56;
  }
  WIN_X = PX - WIN_W / 2;
}

// orientation (motion sensor): 0 upright, 1 sideways = weather (turned right),
// 2 sideways = clock (turned left), 3 upside down (normal layout, rotated 180)
int  orient = 0;

// isSideways(): true when the screen is in one of the two landscape layouts (480 x 320).
bool isSideways() { return orient == 1 || orient == 2; }
bool orientAuto = true, orientFlip = false, orientSwap = false, imuOk = false;
float accX = 0, accY = 0, accZ = 0;
const int   NEST_Y = 252;               // Nest panel starts here

// ============================================================================
// SETTINGS / STATE
// ----------------------------------------------------------------------------
// Global state. Settings that survive power cuts are kept in flash with
// Preferences (namespace "clocknest"); the key name is next to each one where
// it is loaded in setup(). Variables shared with other tasks are 'volatile'
// or protected by nestLock.
// ============================================================================
Preferences prefs;
String wifiSsid, wifiPass;
bool wifiOn = true;                      // Wi-Fi switched on (Settings > Wi-Fi)
bool btOn = false;                       // Bluetooth switched on (Settings > Bluetooth)
unsigned long wifiJoinStart = 0;         // when the last connect attempt began
// BLE (Nordic UART service) - set up in the Bluetooth section
NimBLECharacteristic* txChar = nullptr;
volatile bool bleConnected = false, bleSecure = false, bleHasCmd = false;
String bleInbox;
// clock face: the jump-hour retrograde dial, a classic analog dial, or a big
// seven-segment digital clock. Same corners (icons, date, bell) on all three.
const int FACE_JUMP = 0, FACE_ANALOG = 1, FACE_DIGITAL = 2, N_FACES = 3;
const char* FACE_NAMES[N_FACES] = { "JUMP", "ANALOG", "DIGITAL" };
int faceStyle = FACE_JUMP;               // saved as "face"
const unsigned long SETTINGS_IDLE_MS = 20000;   // settings slide away after 20 s without a touch
bool holdPush = false;                   // draw a frame but don't show it yet (slide animations)
bool swipeArmed = false;                 // a finger went down where a swipe can start
int  swipeY0 = 0;                        // ... at this height
int  backlight = 255;                    // brightness in use now (PWM 0-255)
int  blPct = 80;                         // brightness on battery, % (10% steps); USB = full
unsigned long blPreviewUntil = 0;        // Display page: show the battery level briefly
// sleep on battery (Settings > Display)
const int SLEEP_OPTS[] = { 0, 15, 30, 60, 120, 300, 600 };   // seconds, 0 = never
const char* SLEEP_LABELS[] = { "Never", "15 s", "30 s", "1 min", "2 min", "5 min", "10 min" };
const int N_SLEEP = 7;
int  sleepIdx = 3;                       // 1 min
bool keepWifiAsleep = true, keepBtAsleep = false;
bool wakeOnMotion = true;                // tap / move / shake wakes the screen (Display page)
float motRefX = 0, motRefY = 0, motRefZ = 0;   // which way gravity pointed when it fell asleep
// power statistics since boot (command: power)
unsigned long statAsleepMs = 0, statSleepStart = 0;
uint32_t statWakeTouch = 0, statWakeMotion = 0, statWakeAlarm = 0, statWakeUsb = 0;
bool sleeping = false, sleptWifi = false, sleptBt = false;

const char* TZ_INFO = "PST8PDT,M3.2.0,M11.1.0";   // US Pacific with DST
const char* NTP1 = "pool.ntp.org";
const char* NTP2 = "time.nist.gov";

volatile bool sntpDone = false;
time_t lastSyncOk = 0;
bool rtcFound = false, rtcValidAtBoot = false;

bool demoActive = false;                // demo: hh:59:55, back after 8 s
long demoOffset = 0;
unsigned long demoStart = 0;

int shownHour = -1, shownSec = -1;
float shownHand = 0;

const int SCREEN_FACE = 0, SCREEN_SETTINGS = 1, SCREEN_MENU = 2, SCREEN_SD = 3, SCREEN_WX = 4,
          SCREEN_WIFI = 5, SCREEN_KBD = 6, SCREEN_BT = 7, SCREEN_DISP = 8, SCREEN_USB = 9,
          SCREEN_SOUND = 10;                                                            // SETTINGS = the Clock page
int screen = SCREEN_FACE;
unsigned long lastTouchMs = 0;
int  editH = 0, editM = 0;
int  alarmH = 7, alarmM = 0;
bool alarmOn = false;
int  alarmVol = 80;                      // alarm volume, % (Clock page)
volatile int previewBeeps = 0;           // beeps to play after a volume change
String alarmFile;                        // alarm sound on the SD card ("" = beep)
volatile bool alarmMusic = false;        // the alarm is playing alarmFile
unsigned long savedMsgUntil = 0;
const char* savedMsg = "";
uint16_t savedMsgColor = 0x07E0;
volatile bool alarmRinging = false;
unsigned long alarmStartMs = 0, alarmLimitMs = 15000;
long lastAlarmKey = -1;
// hourly chime (Settings > Sound) - see the CHIME section
bool chimeOn = true;                     // "chOn"
int  chimeVol = 50;                      // "chVol", %, 5% steps (the alarm has its own)
String chimeFile;                        // "chSnd": a file on the SD card ("" = built-in bell)
bool quietOn = true;                     // "chQt"
int  quietFrom = 22, quietTo = 7;        // "chQF" / "chQT": no chime from 22:00 until 07:00
long lastChimeKey = -1;                  // day * 24 + hour of the last chime (never twice an hour)

// buttons: settings screen + Nest panel
enum { B_NONE = 0, B_DONE, B_TH_UP, B_TH_DN, B_TM_UP, B_TM_DN, B_SET,
       B_AH_UP, B_AH_DN, B_AM_UP, B_AM_DN, B_AON, B_TEST, B_SOUND, B_VDN, B_VUP, B_WSYNC,
       B_N_UP, B_N_DN, B_M_HEAT, B_M_COOL, B_M_AUTO, B_M_OFF, B_M_ECO,
       B_BACK, B_TILE_CLOCK, B_TILE_SD,
       B_SD_UP, B_SD_DN, B_SD_REFRESH, B_SD_FORMAT, B_SD_CANCEL, B_SD_YES, B_SD_USB, B_USB_DONE,
       B_TILE_WX, B_K0, B_K1, B_K2, B_K3, B_K4, B_K5, B_K6, B_K7, B_K8, B_K9, B_KDEL, B_KOK,
       B_TILE_WIFI, B_TILE_BT, B_TOGGLE, B_W_UP, B_W_DN, B_W_SCAN, B_KB_SHOW,
       B_BT_NEWPIN, B_BT_FORGET, B_BT_DISC,
       B_TILE_DISP, B_D_BLDN, B_D_BLUP, B_D_SLPDN, B_D_SLPUP, B_D_KWIFI, B_D_KBT, B_D_KMOT, B_FACE,
       B_TILE_SOUND, B_TILE_RADIO, B_C_ON, B_C_VDN, B_C_VUP, B_C_TEST, B_C_SND,
       B_Q_ON, B_QF_UP, B_QF_DN, B_QT_UP, B_QT_DN };
// settings grid: 2 columns x 4 rows of tiles; add new pages here
const int TILE_W = 140, TILE_H = 90;
const int TILE_X[2] = { 15, 165 };
const int TILE_Y[4] = { 62, 160, 258, 356 };
const Btn MBTNS[] = {
  { B_DONE, 210, 8, 100, 44 },
  { B_TILE_CLOCK, 15, 62, TILE_W, TILE_H },  { B_TILE_DISP, 165, 62, TILE_W, TILE_H },
  { B_TILE_WX, 15, 160, TILE_W, TILE_H },    { B_TILE_WIFI, 165, 160, TILE_W, TILE_H },
  { B_TILE_BT, 15, 258, TILE_W, TILE_H },    { B_TILE_SD, 165, 258, TILE_W, TILE_H },
  { B_TILE_SOUND, 15, 356, TILE_W, TILE_H }, // (Radio, 165 / 356: placeholder, not a button yet)
};
const int NMB = sizeof(MBTNS) / sizeof(MBTNS[0]);
const Btn SBTNS[] = {                   // Clock page (portrait)
  { B_BACK, 10, 8, 96, 44 },  { B_FACE, 214, 8, 96, 44 },
  { B_TH_UP, 45, 88, 70, 40 },  { B_TH_DN, 45, 196, 70, 40 },
  { B_TM_UP, 135, 88, 70, 40 }, { B_TM_DN, 135, 196, 70, 40 },
  { B_SET, 226, 94, 78, 44 },  { B_WSYNC, 226, 170, 78, 56 },
  { B_AH_UP, 45, 292, 70, 40 }, { B_AH_DN, 45, 400, 70, 40 },
  { B_AM_UP, 135, 292, 70, 40 }, { B_AM_DN, 135, 400, 70, 40 },
  { B_AON, 226, 296, 78, 44 },
  { B_VDN, 226, 368, 37, 40 }, { B_VUP, 267, 368, 37, 40 },
  { B_SOUND, 110, 263, 198, 28 },        // the sound name at the panel's top right
};
const Btn NBTNS[] = {                   // Nest panel on the face
  { B_N_UP, 222, 290, 88, 44 }, { B_N_DN, 222, 374, 88, 44 },
  { B_M_HEAT, 4, 428, 59, 46 }, { B_M_COOL, 67, 428, 59, 46 }, { B_M_AUTO, 130, 428, 59, 46 },
  { B_M_OFF, 193, 428, 59, 46 }, { B_M_ECO, 256, 428, 60, 46 },
};
const int NSB = sizeof(SBTNS) / sizeof(SBTNS[0]);
const int NNB = sizeof(NBTNS) / sizeof(NBTNS[0]);
int pressedBtn = B_NONE;

// ---- Nest ----
String nestProject, nestClient, nestSecret, nestRefresh;
NestState nest;                          // shared with the Nest task (guarded)
SemaphoreHandle_t nestLock;
volatile int nestVersion = 0;            // bumps when new data arrives
volatile bool nestPollSoon = true;
// command from the UI to the Nest task
volatile int  cmdType = 0;               // 0 none, 1 setpoint, 2 mode, 3 eco
float cmdHeatC = 0, cmdCoolC = 0;
char  cmdMode[12] = "";
// target being adjusted on screen (sent 1.5 s after the last tap)
bool  pendingSet = false;
float pendHeatC = 0, pendCoolC = 0;
unsigned long pendLastTap = 0;
bool  sentShow = false;                  // show the sent target until Google re-reads
int   sentVersion = 0;
unsigned long nestMsgUntil = 0;

// ---- weather ----
// one weather reading: current conditions + up to 7 forecast days
// (temperatures in Celsius; converted to F only when drawn)
struct WxState {
  bool  valid = false;
  float curC = 0, windKmh = 0;
  int   curCode = 0, curHum = -1;
  bool  isDay = true;
  int   n = 0;                           // forecast days (up to 7)
  int   wday[7] = { 0 };                 // 0 = Sunday
  int   code[7] = { 0 };
  float hiC[7] = { 0 }, loC[7] = { 0 };
  int   pop[7] = { -1, -1, -1, -1, -1, -1, -1 };   // rain chance %
  unsigned long updatedMs = 0;
  char  error[48] = "";
};
WxState wx;                              // shared with the network task (nestLock)
float wxLat = 0, wxLon = 0;
bool  wxHaveLoc = false;
String wxPlace;
char  wxQuery[64] = "";                  // city or ZIP to look up (network task)
volatile int wxLookup = 0;               // 0 idle, 1 looking up, 2 found, 3 failed
char  wxLookupMsg[48] = "";
volatile bool wxSoon = false;
int   panelMode = 1;                     // bottom panel: 0 Nest, 1 weather (home)
unsigned long nestShownAt = 0;           // when the Nest panel was opened
const char* ringLabel = "ALARM";         // (left from the old pop-up; not shown any more)
char  nestMsg[48] = "";

// ============================================================================
// I2C: EXPANDER, TOUCH, RTC
// ----------------------------------------------------------------------------
// One I2C bus (SDA 8, SCL 7, 400 kHz) carries: TCA9554 expander 0x20,
// FT6336 touch 0x38, PCF85063 RTC 0x51, QMI8658 motion 0x6B, AXP2101 power
// 0x34, ES8311 audio 0x18. Rule: only the main task uses I2C (the Wire
// library isn't safe to share between tasks).
// ============================================================================
void i2cWrite(uint8_t a, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(a); Wire.write(reg); Wire.write(val); Wire.endTransmission();
}

// i2cRead(): Reads n bytes starting at register 'reg' of I2C device 'a'.
//   Returns how many bytes arrived (n on success, 0 if the device didn't answer).
//   Uses a repeated start (endTransmission(false)) as these chips expect.
int i2cRead(uint8_t a, uint8_t reg, uint8_t* buf, int n) {
  Wire.beginTransmission(a); Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0;
  int got = Wire.requestFrom((int)a, n);
  for (int i = 0; i < got; i++) buf[i] = Wire.read();
  return got;
}

uint8_t expOut = 0x7F;                  // all high except P7 (speaker amp off)

// lcdHardReset(): Pulses the display's RESET line. On this board it isn't a GPIO: it is pin
//   P1 of the TCA9554 I/O expander, so we set it over I2C. Also sets which
//   expander pins are outputs (LCD reset, SD CS, speaker amp...).
void lcdHardReset() {
  i2cWrite(EXP_ADDR, 1, expOut);
  i2cWrite(EXP_ADDR, 3, 0b01110100);    // P0,P1,P3,P7 outputs
  expOut &= ~(1 << 1); i2cWrite(EXP_ADDR, 1, expOut); delay(20);
  expOut |=  (1 << 1); i2cWrite(EXP_ADDR, 1, expOut); delay(120);
}

// first finger, in screen coordinates (false = no touch).
// Portrait (rotation 0): the touch chip's coordinates match the screen.
bool readTouch(int& x, int& y) {
  uint8_t d[5];
  if (i2cRead(TOUCH_ADDR, 0x02, d, 5) != 5) return false;
  int n = d[0] & 0x0F;
  if (n < 1 || n > 2) return false;
  int rx = ((d[1] & 0x0F) << 8) | d[2];
  int ry = ((d[3] & 0x0F) << 8) | d[4];
  x = rx;
  y = ry;
  if (orient == 3) { x = 319 - rx; y = 479 - ry; }   // upside down
  return true;
}

// ============================================================================
// AUDIO: ES8311 CODEC + I2S, AMP ON EXPANDER P7
// ----------------------------------------------------------------------------
// Sound path: ESP32-S3 I2S (48 kHz, 16-bit stereo, MCLK = 256 x 48 kHz) ->
// ES8311 codec (configured over I2C) -> speaker amplifier (switched by
// expander pin P7). The beep is generated here; MP3 alarm sound comes from the
// audio library further down and is fed into the same I2S output.
// ============================================================================
I2SClass i2s;
const uint8_t CODEC_ADDR = 0x18;
const int SAMPLE_RATE = 48000;          // MCLK = 256 x 48 kHz = 12.288 MHz (same codec ratio)
bool audioOk = false;

// codecRead(): Reads one register of the ES8311 audio codec (used for read-modify-write).
uint8_t codecRead(uint8_t reg) {
  uint8_t v = 0;
  i2cRead(CODEC_ADDR, reg, &v, 1);
  return v;
}

// ampOn(): Switches the speaker amplifier (expander pin P7) on or off. Only ever called
//   from the main task, because I2C must not be used from two tasks at once.
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

// alarm volume %: 0 = mute, 10..100 % = -40 dB .. +6 dB (0xBF is 0 dB)
void setAlarmVolume(int pct) {
  int reg = 0;
  if (pct > 0) {
    float db = -40.0f + (pct - 10) * 46.0f / 90.0f;
    reg = constrain((int)lroundf(0xBF + 2 * db), 0, 0xFF);
  }
  i2cWrite(CODEC_ADDR, 0x32, reg);
}
bool clockSyncAsked = false;             // Wi-Fi SYNC pressed on the Clock page
unsigned long volChangedAt = 0;          // pending "three beeps" preview
unsigned long previewAmpUntil = 0;

// playMs(): Plays a 10 ms buffer repeatedly for 'ms' milliseconds, but stops early as
//   soon as the alarm is cancelled. Used by the beep pattern.
void playMs(const int16_t* buf, int ms) {
  for (int i = 0; i < ms / 10 && alarmRinging; i++)
    i2s.write((const uint8_t*)buf, sizeof(toneBuf));
}

// ---- the built-in chime: a two-note bell, made once in PSRAM ----
// Two bell notes (E6, then C6 0.55 s later), each a few overtones that die
// away like a struck bell; 2.6 s, 48 kHz mono. Same sound as the Kindle Home
// Hub's chime. Built by the beep task the first time it is needed.
// Each overtone is a rotating phasor with a multiplying decay - no sinf/expf
// per sample: calling those ~2 million times kept core 0 busy for seconds,
// and the task watchdog reset the board.
const int BELL_LEN = SAMPLE_RATE * 26 / 10;
int16_t* bellPcm = nullptr;
volatile bool chimeBell = false;         // the beep task is playing the bell (set by startChime)
volatile bool chimeMusic = false;        // the audio library is playing chimeFile

void makeBell() {
  float* acc = (float*)ps_malloc(BELL_LEN * sizeof(float));
  bellPcm = (int16_t*)ps_malloc(BELL_LEN * sizeof(int16_t));
  if (!acc || !bellPcm) { free(acc); free(bellPcm); bellPcm = nullptr; return; }
  for (int i = 0; i < BELL_LEN; i++) acc[i] = 0;
  const float notes[2][2] = { { 0.0f, 1318.5f }, { 0.55f, 1046.5f } };       // start s, pitch Hz
  const float parts[4][2] = { { 1.0f, 1.0f }, { 2.0f, 0.35f }, { 2.76f, 0.18f }, { 5.4f, 0.06f } };
  const float attack = SAMPLE_RATE * 0.004f;                  // 4 ms attack (no click)
  for (int n = 0; n < 2; n++) {
    int i0 = (int)(notes[n][0] * SAMPLE_RATE);
    for (int p = 0; p < 4; p++) {
      float w = 2 * PI * notes[n][1] * parts[p][0] / SAMPLE_RATE;
      float cr = cosf(w), sr = sinf(w), c = 1, sn = 0;          // phasor: (c, sn) turns by w each sample
      float amp = parts[p][1];
      float dec = expf(-(2.2f + parts[p][0] * 0.9f) / SAMPLE_RATE);   // bell + overtone decay per sample
      for (int i = i0; i < BELL_LEN && amp > 1e-5f; i++) {
        int k = i - i0;
        acc[i] += amp * sn * (k < attack ? k / attack : 1.0f);
        amp *= dec;
        float nc = c * cr - sn * sr;
        sn = sn * cr + c * sr;
        c = nc;
        if ((k & 1023) == 1023) { float m = 1.0f / sqrtf(c * c + sn * sn); c *= m; sn *= m; }   // keep length 1
      }
      vTaskDelay(1);                                           // let the idle task run
    }
  }
  float peak = 0.001f;
  for (int i = 0; i < BELL_LEN; i++) peak = fmaxf(peak, fabsf(acc[i]));
  for (int i = 0; i < BELL_LEN; i++) bellPcm[i] = (int16_t)(acc[i] / peak * 0.8f * 32767);
  free(acc);
}

// playBell(): writes the bell to I2S (as stereo, 10 ms at a time); stops at
//   once if the alarm starts or the chime is cancelled.
void playBell() {
  if (!bellPcm) makeBell();
  if (bellPcm) {
    static int16_t st[SAMPLE_RATE / 100 * 2];
    const int N = SAMPLE_RATE / 100;
    for (int i = 0; i + N <= BELL_LEN && chimeBell && !alarmRinging; i += N) {
      for (int k = 0; k < N; k++) st[2 * k] = st[2 * k + 1] = bellPcm[i + k];
      i2s.write((const uint8_t*)st, sizeof(st));
    }
  }
  for (int i = 0; i < 4; i++) i2s.write((const uint8_t*)silenceBuf, sizeof(silenceBuf));
  chimeBell = false;                     // done: loop() switches the amp off
}

// beep pattern while alarmRinging: 4 short beeps, a pause, repeat
void beepTask(void*) {
  for (;;) {
    if (chimeBell && !alarmRinging) { playBell(); continue; }
    if (previewBeeps > 0 && !alarmRinging) {  // volume preview: beep beep beep
      for (int b = 0; b < 3; b++) {
        for (int i = 0; i < 12; i++) i2s.write((const uint8_t*)toneBuf, sizeof(toneBuf));      // 120 ms
        for (int i = 0; i < 12; i++) i2s.write((const uint8_t*)silenceBuf, sizeof(silenceBuf));
      }
      previewBeeps = 0;
      continue;
    }
    if (!alarmRinging || alarmMusic) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }
    while (alarmRinging && !alarmMusic) {
      for (int b = 0; b < 4 && alarmRinging && !alarmMusic; b++) { playMs(toneBuf, 100); playMs(silenceBuf, 80); }
      playMs(silenceBuf, 500);
    }
    if (!alarmMusic) for (int i = 0; i < 4; i++) i2s.write((const uint8_t*)silenceBuf, sizeof(silenceBuf));
  }
}

// audioInit(): Builds the 2 kHz beep and silence buffers, configures the codec, starts I2S
//   on the codec pins (BCLK 13, LRCK 15, DOUT 16, DIN 14, MCLK 12) and starts the
//   beep task. Leaves audioOk false if anything fails (the clock still works).
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

// startAlarm(): Starts the ringing state for limitMs milliseconds: amp on, alarmRinging true.
//   The beep task (or the music player) sees the flag and makes the sound.
void endChime();
void startAlarm(unsigned long limitMs) {
  endChime();                            // the alarm wins over a chime that is still playing
  setAlarmVolume(alarmVol);
  alarmLimitMs = limitMs;
  alarmStartMs = millis();
  ampOn(true);                           // amp switched from here (I2C stays on one task)
  alarmRinging = true;
  Serial.println(audioOk ? "Alarm!" : "Alarm! (no audio - codec/I2S not running)");
}

void stopAlarmMusic();

// stopAlarm(): Ends any ringing (alarm, test or music): stops the music, clears the flag,
//   waits for the last audio buffer to drain, then switches the amp off.
void stopAlarm() {
  if (!alarmRinging) return;
  stopAlarmMusic();
  alarmRinging = false;
  delay(30);                             // let the last buffer finish
  ampOn(false);
  Serial.println("Alarm stopped");
}

// bcd2dec(): RTC registers hold binary-coded decimal; these two convert to/from normal numbers.
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

// rtcWrite(): Writes a UTC time to the PCF85063 RTC (seconds .. year registers, BCD).
//   Clearing the seconds register's top bit also clears the 'oscillator stopped'
//   flag, so the next read is trusted.
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

// ============================================================================
// TIME HELPERS
// ----------------------------------------------------------------------------
// The system clock runs in UTC; local time comes from the TZ rule (US Pacific
// with DST). The RTC chip keeps UTC through power cuts.
// ============================================================================
bool timeValid() { return time(nullptr) > 1700000000; }

// nowLocal(): Fills 't' with the local time (time zone + DST applied). Returns false if the
//   clock has never been set. In demo mode it adds the demo offset.
bool nowLocal(struct tm& t) {
  if (!timeValid()) return false;
  time_t n = time(nullptr) + (demoActive ? demoOffset : 0);
  localtime_r(&n, &t);
  return true;
}

// fmtTime(): Formats a time_t as local 'YYYY-MM-DD HH:MM:SS' for Serial output.
String fmtTime(time_t t) {
  if (t == 0) return "never";
  struct tm lt; localtime_r(&t, &lt);
  char b[32]; strftime(b, sizeof(b), "%a %b %d %H:%M", &lt);
  return String(b);
}

// ============================================================================
// NEST: GOOGLE SMART DEVICE MANAGEMENT API
// ----------------------------------------------------------------------------
// Talks to Google over HTTPS (certificates in google_roots.h). Flow: refresh
// token -> 1-hour access token -> GET the device list (thermostat traits) /
// POST commands. Everything here runs in the network task (nestTask), never
// in the main loop, because each request can take a second or two.
// ============================================================================
// Runs in its own task so HTTPS calls (1-2 s) never freeze the clock.
const char* SDM_HOST = "https://smartdevicemanagement.googleapis.com/v1/enterprises/";
String accessToken;
unsigned long tokenExpiresAt = 0;
String deviceName;                      // enterprises/<project>/devices/<id>

// nestConfigured(): true when all four Nest credentials (project, client id, secret, refresh token)
//   have been stored - only then does the Nest task try to talk to Google.
bool nestConfigured() {
  return nestProject.length() && nestClient.length() && nestSecret.length() && nestRefresh.length();
}

// nestSetError(): Stores an error message for the Nest panel (thread-safe) and asks for a redraw.
void nestSetError(const char* e) {
  xSemaphoreTake(nestLock, portMAX_DELAY);
  strncpy(nest.error, e, sizeof(nest.error) - 1);
  nest.error[sizeof(nest.error) - 1] = 0;
  xSemaphoreGive(nestLock);
  nestVersion++;
  Serial.printf("Nest: %s\n", e);
}

// HTTPS request with Google's root certificates. Returns HTTP code.
int httpsRequest(const String& url, const char* method, const String& body,
                 const char* contentType, bool auth, String& reply, const char* ca) {
  WiFiClientSecure client;
  client.setCACert(ca);
  HTTPClient http;
  http.setTimeout(10000);
  if (!http.begin(client, url)) return -1;
  if (auth) http.addHeader("Authorization", "Bearer " + accessToken);
  if (contentType) http.addHeader("Content-Type", contentType);
  int code;
  if (!strcmp(method, "GET")) code = http.GET();
  else code = http.POST(body);
  reply = code > 0 ? http.getString() : String();
  http.end();
  return code;
}

// refreshAccessToken(): Trades the long-lived refresh token for a 1-hour access token at Google's
//   OAuth endpoint. Runs in the Nest task; on a 400 the token was revoked or
//   expired and nest_auth.py must be run again.
bool refreshAccessToken() {
  String body = "client_id=" + nestClient + "&client_secret=" + nestSecret +
                "&refresh_token=" + nestRefresh + "&grant_type=refresh_token";
  String reply;
  int code = httpsRequest("https://www.googleapis.com/oauth2/v4/token", "POST", body,
                          "application/x-www-form-urlencoded", false, reply, GOOGLE_ROOT_CAS);
  if (code != 200) {
    char e[64];
    snprintf(e, sizeof(e), "sign-in failed (%d)%s", code,
             code == 400 ? " - token expired? rerun nest_auth.py" : "");
    nestSetError(e);
    return false;
  }
  JsonDocument doc;
  if (deserializeJson(doc, reply)) { nestSetError("sign-in: bad reply"); return false; }
  accessToken = doc["access_token"].as<String>();
  int secs = doc["expires_in"] | 3599;
  tokenExpiresAt = millis() + (unsigned long)secs * 1000UL;
  return accessToken.length() > 0;
}

// read the first thermostat in the project
bool fetchThermostat() {
  String reply;
  int code = httpsRequest(String(SDM_HOST) + nestProject + "/devices", "GET", "", nullptr, true, reply, GOOGLE_ROOT_CAS);
  if (code == 401) { tokenExpiresAt = 0; return false; }   // token went stale: refresh next round
  if (code != 200) {
    char e[64];
    snprintf(e, sizeof(e), code == 429 ? "Google rate limit - slowing down" : "read failed (%d)", code);
    nestSetError(e);
    return false;
  }
  JsonDocument doc;
  if (deserializeJson(doc, reply)) { nestSetError("bad device reply"); return false; }
  for (JsonObject d : doc["devices"].as<JsonArray>()) {
    const char* type = d["type"] | "";
    if (strcmp(type, "sdm.devices.types.THERMOSTAT") != 0) continue;
    JsonObject tr = d["traits"];
    NestState s;
    s.valid    = true;
    s.ambientC = tr["sdm.devices.traits.Temperature"]["ambientTemperatureCelsius"] | 0.0f;
    s.humidity = tr["sdm.devices.traits.Humidity"]["ambientHumidityPercent"] | -1;
    strlcpy(s.mode, tr["sdm.devices.traits.ThermostatMode"]["mode"] | "", sizeof(s.mode));
    strlcpy(s.eco,  tr["sdm.devices.traits.ThermostatEco"]["mode"] | "OFF", sizeof(s.eco));
    strlcpy(s.hvac, tr["sdm.devices.traits.ThermostatHvac"]["status"] | "OFF", sizeof(s.hvac));
    JsonObject sp = tr["sdm.devices.traits.ThermostatTemperatureSetpoint"];
    s.heatC = sp["heatCelsius"] | 0.0f;
    s.coolC = sp["coolCelsius"] | 0.0f;
    const char* scale = tr["sdm.devices.traits.Settings"]["temperatureScale"] | "FAHRENHEIT";
    s.scaleF = strcmp(scale, "CELSIUS") != 0;
    s.updatedMs = millis();
    s.error[0] = 0;
    deviceName = d["name"].as<String>();
    xSemaphoreTake(nestLock, portMAX_DELAY);
    nest = s;
    xSemaphoreGive(nestLock);
    nestVersion++;
    return true;
  }
  nestSetError("no thermostat in this project");
  return false;
}

// sendNestCommand(): Sends one SDM executeCommand (e.g. SetHeat, SetMode) to the thermostat.
//   On failure Google's own error message is shown on the panel.
bool sendNestCommand(const String& command, const String& paramsJson) {
  if (deviceName.length() == 0) return false;
  String body = "{\"command\":\"" + command + "\",\"params\":" + paramsJson + "}";
  String reply;
  int code = httpsRequest("https://smartdevicemanagement.googleapis.com/v1/" + deviceName + ":executeCommand",
                          "POST", body, "application/json", true, reply, GOOGLE_ROOT_CAS);
  if (code != 200) {
    // Google explains refusals (e.g. setpoint in ECO) in the reply
    JsonDocument doc;
    const char* msg = "";
    if (!deserializeJson(doc, reply)) msg = doc["error"]["message"] | "";
    char e[64];
    snprintf(e, sizeof(e), "command refused (%d) %s", code, msg);
    nestSetError(e);
    return false;
  }
  return true;
}

// ============================================================================
// WEATHER: OPEN-METEO (FREE, NO KEY)
// ----------------------------------------------------------------------------
// Weather from open-meteo.com: no account or key. A ZIP or 'City, ST' is
// turned into latitude/longitude by their geocoding API once; the forecast
// (now + 7 days) is fetched every 15 minutes by the network task.
// ============================================================================
const char* US_STATES[] = {
  "AL","Alabama","AK","Alaska","AZ","Arizona","AR","Arkansas","CA","California",
  "CO","Colorado","CT","Connecticut","DE","Delaware","FL","Florida","GA","Georgia",
  "HI","Hawaii","ID","Idaho","IL","Illinois","IN","Indiana","IA","Iowa",
  "KS","Kansas","KY","Kentucky","LA","Louisiana","ME","Maine","MD","Maryland",
  "MA","Massachusetts","MI","Michigan","MN","Minnesota","MS","Mississippi","MO","Missouri",
  "MT","Montana","NE","Nebraska","NV","Nevada","NH","New Hampshire","NJ","New Jersey",
  "NM","New Mexico","NY","New York","NC","North Carolina","ND","North Dakota","OH","Ohio",
  "OK","Oklahoma","OR","Oregon","PA","Pennsylvania","RI","Rhode Island","SC","South Carolina",
  "SD","South Dakota","TN","Tennessee","TX","Texas","UT","Utah","VT","Vermont",
  "VA","Virginia","WA","Washington","WV","West Virginia","WI","Wisconsin","WY","Wyoming",
  "DC","District of Columbia" };
const int N_STATES = sizeof(US_STATES) / sizeof(US_STATES[0]) / 2;

// stateAbbr(): US state full name -> 2-letter code (for showing 'Portland, OR').
const char* stateAbbr(const char* name) {
  for (int i = 0; i < N_STATES; i++) if (!strcasecmp(US_STATES[i * 2 + 1], name)) return US_STATES[i * 2];
  return nullptr;
}

// stateName(): US state code -> full name (to match 'OR' against the geocoder's 'Oregon').
const char* stateName(const char* abbr) {
  for (int i = 0; i < N_STATES; i++) if (!strcasecmp(US_STATES[i * 2], abbr)) return US_STATES[i * 2 + 1];
  return nullptr;
}

// wxSetError(): Stores a weather error message (thread-safe) for the panel.
void wxSetError(const char* e) {
  xSemaphoreTake(nestLock, portMAX_DELAY);
  strlcpy(wx.error, e, sizeof(wx.error));
  xSemaphoreGive(nestLock);
  nestVersion++;
  Serial.printf("Weather: %s\n", e);
}

// urlEncode(): Percent-encodes a string for use in a URL query (spaces, commas, etc.).
String urlEncode(const String& v) {
  String o;
  const char* hex = "0123456789ABCDEF";
  for (size_t i = 0; i < v.length(); i++) {
    char c = v[i];
    if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.') o += c;
    else { o += '%'; o += hex[(c >> 4) & 15]; o += hex[c & 15]; }
  }
  return o;
}

// geoFail(): Reports a failed place / ZIP lookup to the Weather settings page and Serial.
void geoFail(const char* m) {
  strlcpy(wxLookupMsg, m, sizeof(wxLookupMsg));
  wxLookup = 3;
  nestVersion++;
  Serial.printf("Weather: %s\n", m);
}

// isZip(): true for exactly five digits (a US ZIP code).
bool isZip(const char* q) {
  if (strlen(q) != 5) return false;
  for (int i = 0; i < 5; i++) if (!isdigit((unsigned char)q[i])) return false;
  return true;
}

// "97201" -> US ZIP code; "Portland, OR" -> Portland, prefer the one in Oregon
void geocodeCity(const char* query) {
  wxLookup = 1;
  nestVersion++;
  bool zip = isZip(query);
  String q(query), name = q, qual;
  int comma = q.indexOf(',');
  if (comma >= 0) { name = q.substring(0, comma); qual = q.substring(comma + 1); }
  if (zip) { name = q; qual = ""; }
  name.trim(); qual.trim();
  String reply;
  int code = httpsRequest("https://geocoding-api.open-meteo.com/v1/search?count=10&language=en&format=json&name=" +
                          urlEncode(name) + (zip ? "&countryCode=US" : ""), "GET", "", nullptr, false, reply, WX_ROOT_CAS);
  if (code != 200) { char e[48]; snprintf(e, sizeof(e), "lookup failed (%d) - try again", code); geoFail(e); return; }
  JsonDocument doc;
  if (deserializeJson(doc, reply)) { geoFail("lookup: bad reply"); return; }
  JsonArray res = doc["results"].as<JsonArray>();
  if (res.isNull() || res.size() == 0) { geoFail(zip ? "ZIP code not found" : "place not found"); return; }
  JsonObject pick = res[0];
  if (zip) {                             // prefer the result that lists this ZIP
    bool hit = false;
    for (JsonObject r : res) {
      for (JsonVariant pc : r["postcodes"].as<JsonArray>())
        if (!strcmp(pc | "", query)) { pick = r; hit = true; break; }
      if (hit) break;
    }
  }
  if (qual.length()) {
    const char* full = stateName(qual.c_str());
    bool found = false;
    for (JsonObject r : res) {
      const char* a1 = r["admin1"] | "";
      const char* cc = r["country_code"] | "";
      const char* cn = r["country"] | "";
      if (!strcasecmp(a1, qual.c_str()) || (full && !strcasecmp(a1, full)) ||
          !strcasecmp(cc, qual.c_str()) || !strcasecmp(cn, qual.c_str())) { pick = r; found = true; break; }
    }
    if (!found) Serial.println("Weather: no match for '" + qual + "', using the first result");
  }
  float lat = pick["latitude"] | 0.0f, lon = pick["longitude"] | 0.0f;
  const char* a1 = pick["admin1"] | "";
  const char* cc = pick["country_code"] | "";
  const char* ab = !strcmp(cc, "US") ? stateAbbr(a1) : nullptr;
  String place = String(pick["name"] | "?") + ", " + (ab ? ab : cc);
  wxLat = lat; wxLon = lon; wxPlace = place; wxHaveLoc = true;
  prefs.putFloat("wxLat", lat);
  prefs.putFloat("wxLon", lon);
  prefs.putString("wxPlace", place);
  if (zip) prefs.putString("wxZip", query);
  snprintf(wxLookupMsg, sizeof(wxLookupMsg), "Found: %s", place.c_str());
  wxLookup = 2;
  nestVersion++;
  Serial.printf("Weather: %s (%.3f, %.3f) - %s %s\n", place.c_str(), lat, lon, pick["admin1"] | "", pick["country"] | "");
  wxSoon = true;
}

// fetchWeather(): Downloads current conditions + a 7-day forecast from Open-Meteo for the
//   saved latitude/longitude and stores them in 'wx' (under nestLock).
//   Values are kept in Celsius; conversion happens when drawing.
bool fetchWeather() {
  char url[400];
  snprintf(url, sizeof(url),
           "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f"
           "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m,is_day"
           "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
           "&timezone=auto&forecast_days=7", wxLat, wxLon);
  String reply;
  int code = httpsRequest(url, "GET", "", nullptr, false, reply, WX_ROOT_CAS);
  if (code != 200) { char e[48]; snprintf(e, sizeof(e), "weather failed (%d)", code); wxSetError(e); return false; }
  JsonDocument doc;
  if (deserializeJson(doc, reply)) { wxSetError("weather: bad reply"); return false; }
  WxState w;
  JsonObject cur = doc["current"];
  w.curC = cur["temperature_2m"] | 0.0f;
  w.curHum = cur["relative_humidity_2m"] | -1;
  w.curCode = cur["weather_code"] | 0;
  w.windKmh = cur["wind_speed_10m"] | 0.0f;
  w.isDay = (cur["is_day"] | 1) != 0;
  JsonObject d = doc["daily"];
  JsonArray days = d["time"];
  w.n = min((int)days.size(), 7);
  for (int i = 0; i < w.n; i++) {
    int y = 0, m = 0, dd = 0;
    sscanf(days[i] | "1970-01-01", "%d-%d-%d", &y, &m, &dd);
    long dn = daysFromCivil(y, m, dd);
    w.wday[i] = (int)(((dn % 7) + 11) % 7);            // 1970-01-01 was a Thursday
    w.code[i] = d["weather_code"][i] | 0;
    w.hiC[i] = d["temperature_2m_max"][i] | 0.0f;
    w.loC[i] = d["temperature_2m_min"][i] | 0.0f;
    w.pop[i] = d["precipitation_probability_max"][i] | -1;
  }
  w.valid = w.n > 0;
  w.updatedMs = millis();
  xSemaphoreTake(nestLock, portMAX_DELAY);
  wx = w;
  xSemaphoreGive(nestLock);
  nestVersion++;
  return true;
}

// weatherService(): Called by the network task: runs a pending place/ZIP lookup, then refreshes
//   the forecast every 15 minutes (or right away when wxSoon is set).
//   On failure it retries after 2 minutes; after 3 failures in a row it rejoins Wi-Fi.
void weatherService() {                  // called from the network task
  static unsigned long lastWx = 0;
  if (wxQuery[0]) {
    char q[64];
    strlcpy(q, wxQuery, sizeof(q));
    wxQuery[0] = 0;
    geocodeCity(q);
  }
  if (!wxHaveLoc) return;
  if (wxSoon || lastWx == 0 || millis() - lastWx > 15UL * 60UL * 1000UL) {
    wxSoon = false;
    lastWx = millis();
    static int fails = 0;
    if (fetchWeather()) fails = 0;
    else {
      lastWx = millis() - 13UL * 60UL * 1000UL;   // retry in 2 min
      // Wi-Fi can report "connected" yet pass no traffic (seen after power-up).
      // Three failures in a row (~6 min): drop the link and join again.
      if (++fails >= 3) {
        fails = 0;
        Serial.println("Weather keeps failing: rejoining Wi-Fi");
        WiFi.reconnect();
      }
    }
  }
}

// nestTask(): The network task (FreeRTOS, core 0). Everything slow and HTTPS lives here so
//   the display never freezes: weather service, Nest access-token refresh, Nest
//   commands from the touch screen, and polling the thermostat once a minute
//   (or 1.5 s after a command).
void nestTask(void*) {
  unsigned long lastPoll = 0;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(200));
    if (WiFi.status() != WL_CONNECTED) continue;
    weatherService();
    if (!nestConfigured()) continue;
    if (accessToken.length() == 0 || (long)(tokenExpiresAt - millis()) < 60000) {
      if (!refreshAccessToken()) { vTaskDelay(pdMS_TO_TICKS(30000)); continue; }
    }
    if (cmdType) {
      int t = cmdType;
      char curMode[12];
      xSemaphoreTake(nestLock, portMAX_DELAY);
      strlcpy(curMode, nest.mode, sizeof(curMode));
      xSemaphoreGive(nestLock);
      char p[96];
      bool ok = false;
      if (t == 1) {                      // target temperature for the current mode
        if (!strcmp(curMode, "HEAT")) {
          snprintf(p, sizeof(p), "{\"heatCelsius\":%.2f}", cmdHeatC);
          ok = sendNestCommand("sdm.devices.commands.ThermostatTemperatureSetpoint.SetHeat", p);
        } else if (!strcmp(curMode, "COOL")) {
          snprintf(p, sizeof(p), "{\"coolCelsius\":%.2f}", cmdCoolC);
          ok = sendNestCommand("sdm.devices.commands.ThermostatTemperatureSetpoint.SetCool", p);
        } else if (!strcmp(curMode, "HEATCOOL")) {
          snprintf(p, sizeof(p), "{\"heatCelsius\":%.2f,\"coolCelsius\":%.2f}", cmdHeatC, cmdCoolC);
          ok = sendNestCommand("sdm.devices.commands.ThermostatTemperatureSetpoint.SetRange", p);
        }
      } else if (t == 2) {
        snprintf(p, sizeof(p), "{\"mode\":\"%s\"}", cmdMode);
        ok = sendNestCommand("sdm.devices.commands.ThermostatMode.SetMode", p);
      } else if (t == 3) {
        snprintf(p, sizeof(p), "{\"mode\":\"%s\"}", cmdMode);
        ok = sendNestCommand("sdm.devices.commands.ThermostatEco.SetMode", p);
      }
      cmdType = 0;
      if (ok) Serial.println("Nest: command sent");
      vTaskDelay(pdMS_TO_TICKS(1500));   // give the thermostat a moment, then re-read
      nestPollSoon = true;
    }
    if (nestPollSoon || millis() - lastPoll >= 60000UL) {
      nestPollSoon = false;
      lastPoll = millis();
      fetchThermostat();
    }
  }
}

String fitText(const String& s, int maxW, bool keepEnd);   // (SD section)

// ---- display helpers for the Nest values ----
float cToShown(float c, bool f) { return f ? c * 9.0f / 5.0f + 32.0f : c; }
float shownToC(float v, bool f) { return f ? (v - 32.0f) * 5.0f / 9.0f : v; }

// fmtTemp(): Formats a Celsius value for the Nest panel: whole degrees in F, halves in C.
String fmtTemp(float c, bool f) {        // whole degrees in F, halves in C
  char b[12];
  if (f) snprintf(b, sizeof(b), "%d", (int)lroundf(cToShown(c, true)));
  else   snprintf(b, sizeof(b), "%.1f", roundf(c * 2) / 2);
  return String(b);
}

// ============================================================================
// WI-FI + NTP
// ----------------------------------------------------------------------------
// Wi-Fi stays connected (the Nest and weather need it). NTP sets the clock at
// power-up and every 24 h; each sync is also written to the RTC.
// ============================================================================
void onSntpSync(struct timeval*) { sntpDone = true; }

// wifiStart(): Starts (or restarts) connecting to the saved network, unless Wi-Fi is
//   switched off. Auto-reconnect is on, so drop-outs heal by themselves.
void wifiStart() {
  if (!wifiOn) return;
  wifiJoinStart = millis();
  if (wifiSsid.length() == 0) { Serial.println("No Wi-Fi set. Send: ssid <name>, then pass <password>"); return; }
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  Serial.println("Wi-Fi: connecting to '" + wifiSsid + "'");
}

// ============================================================================
// DRAWING
// ----------------------------------------------------------------------------
// All drawing goes into 'frame', a full-screen sprite in PSRAM, which is
// then pushed to the display in one go (render()). Drawing off-screen means
// no flicker even though the whole frame is redrawn every second.
// ============================================================================
void polar(float deg, float r, int& x, int& y) {
  float a = deg * (PI / 180.0f);
  x = PX + (int)lroundf(r * sinf(a));
  y = PY - (int)lroundf(r * cosf(a));
}

// minuteToDeg(): Maps minutes 0..60 onto the fan: -58 deg (00) .. +58 deg (60).
float minuteToDeg(float m) { return -SPAN + m * (2 * SPAN / 60.0f); }

// hourShown(): Hour as displayed (always 24-hour: 00-23).
int hourShown(int h) { return h; }

// drawFan(): Draws the retrograde dial: the arc, 61 minute ticks (every 5th long and
//   white) and the 00 / 15 / 30 / 45 / 60 labels.
void drawFan() {
  frame.drawArc(PX, PY, R_ARC + 1, R_ARC, 270 - SPAN, 270 + SPAN, C_ARC);
  for (int m = 0; m <= 60; m++) {
    bool five = (m % 5 == 0);
    int x1, y1, x2, y2;
    float deg = minuteToDeg(m);
    polar(deg, five ? R_T5 : R_T1, x1, y1);
    polar(deg, R_TOUT, x2, y2);
    if (five) frame.drawWideLine(x1, y1, x2, y2, TICK5_W, C_TICK5);
    else      frame.drawLine(x1, y1, x2, y2, C_TICK);
  }
  frame.setFont(labelFont);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_center);
  const char* lab[5] = { "00", "15", "30", "45", "60" };
  for (int i = 0; i < 5; i++) {
    int x, y;
    polar(minuteToDeg(i * 15), R_LABEL, x, y);
    frame.drawString(lab[i], x, y);
  }
}

// drawSeconds(): Draws the 60-segment seconds track; segments up to 'sec' are lit cyan.
void drawSeconds(int sec) {
  for (int i = 0; i < 60; i++) {
    float deg = minuteToDeg(i + 0.5f);
    float half = (2 * SPAN / 60.0f) * 0.32f;
    frame.fillArc(PX, PY, R_S_OUT, R_S_IN, deg + 270 - half, deg + 270 + half,
                  i <= sec ? C_SECON : C_SECOFF);
  }
}

// drawHand(): Draws the minute hand as a thin triangle from the hub out to R_HAND.
void drawHand(float deg) {
  int tx, ty;
  polar(deg, R_HAND, tx, ty);
  float a = deg * (PI / 180.0f);
  float bx = PX + R_HAND0 * sinf(a), by = PY - R_HAND0 * cosf(a);
  float nx = cosf(a) * W_HAND / 2, ny = sinf(a) * W_HAND / 2;
  frame.fillTriangle(tx, ty, (int)(bx - nx), (int)(by - ny), (int)(bx + nx), (int)(by + ny), C_HAND);
}

// drawHourWindow(): Draws the amber hour digits in their window. 'roll' 0..1 animates the jump:
//   the old hour slides up and out while the new one slides in from below
//   (clipped to the window).
void drawHourWindow(int from, int to, float roll, bool known) {
  frame.drawRoundRect(WIN_X - WIN_PAD, WIN_Y - WIN_PAD, WIN_W + 2 * WIN_PAD, WIN_H + 2 * WIN_PAD, 2 * WIN_PAD, C_WINRIM);
  frame.fillRect(WIN_X, WIN_Y, WIN_W, WIN_H, C_BG);
  frame.setClipRect(WIN_X, WIN_Y, WIN_W, WIN_H);
  frame.setFont(hourFont);
  frame.setTextColor(C_HOUR);
  frame.setTextDatum(middle_center);
  int cx = WIN_X + WIN_W / 2, cy = WIN_Y + WIN_H / 2 + 2;
  char b[4];
  if (!known) frame.drawString("--", cx, cy);
  else {
    int off = (int)(roll * WIN_H);
    snprintf(b, sizeof(b), "%02d", hourShown(from));
    frame.drawString(b, cx, cy - off);
    if (roll > 0) {
      snprintf(b, sizeof(b), "%02d", hourShown(to));
      frame.drawString(b, cx, cy - off + WIN_H);
    }
  }
  frame.clearClipRect();
}

const char* WD[7] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
const char* MO[12] = { "JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC" };

void drawAlarmBell(int cx, int cy, float sc, uint16_t c);   // (defined with the status icons)
void drawChimeIcon(int cx, int cy, float sc);                // (defined with the status icons)
bool ringBlink();                                            // (defined further down)

// drawDate(): Weekday at the bottom left, day at the bottom right with the
//   month above it - and the alarm bell, centered over the weekday's second
//   letter, halfway up to the hour box (amber = on, flashing while ringing, none =
//   off). Jump hour: bell and chime note side by side instead (see below).
void drawDate(const struct tm& t, bool known) {
  frame.setTextColor(C_DATE);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextDatum(bottom_left);                       // weekday, bottom left
  const char* wd = known ? WD[t.tm_wday] : "---";
  frame.drawString(wd, 10, 236);
  // vertically: halfway between the top line of the hour box and the top of the weekday
  int by = ((WIN_Y - WIN_PAD) + (236 - frame.fontHeight())) / 2;
  float isc = 1.0f;
  int bx;
  if (faceStyle == FACE_JUMP) {
    // jump hour: bell and chime note (20 px tall) across the width of "Tue":
    // bell's left edge at the text's left, note's right edge at the end of
    // "Tue" - the same every day
    float nsc = 20.0f / 23.0f;                                   // note: 23 px tall at scale 1
    isc = 20.0f / 26.0f;                                         // bell: 26 px tall, 23 wide at scale 1
    bx = 10 + (int)lroundf(11.5f * isc);
    int nx = (alarmOn || alarmRinging) ? 10 + frame.textWidth("Tue") - (int)lroundf(9 * nsc)
                                       : 10 + (int)lroundf(9 * nsc);   // no bell: the note takes its place
    drawChimeIcon(nx, by, nsc);                                  // (note spans cx-9..cx+9)
  } else {
    // analog: the bell alone, centered over the weekday's second letter
    char c1[2] = { wd[0], 0 }, c2[2] = { wd[1], 0 };
    bx = 10 + frame.textWidth(c1) + frame.textWidth(c2) / 2;
  }
  if (alarmRinging) { if (ringBlink()) drawAlarmBell(bx, by, 1.2f * isc, C_HOUR); }   // ringing: flash
  else if (alarmOn) drawAlarmBell(bx, by, isc, C_HOUR);       // alarm off: no bell
  frame.setTextDatum(bottom_right);                      // day, bottom right
  char b[4];
  snprintf(b, sizeof(b), "%02d", t.tm_mday);
  frame.drawString(known ? b : "--", W - 10, 236);
  frame.setFont(&fonts::DejaVu12);                       // month above the day
  frame.drawString(known ? MO[t.tm_mon] : "---", W - 10, 208);
}

// drawGear(): Settings gear: 8 teeth around a disc with a hole (not used since
//   settings open with a swipe; kept in case a gear is wanted again).
void drawGear(int cx, int cy, uint16_t c) {
  for (int i = 0; i < 8; i++) {
    float a = i * PI / 4;
    frame.fillCircle(cx + (int)(11 * cosf(a)), cy + (int)(11 * sinf(a)), 4, c);
  }
  frame.fillCircle(cx, cy, 10, c);
  frame.fillCircle(cx, cy, 4, C_BG);
}

// drawBell(): Older small bell icon (kept for reference; drawAlarmBell is the one in use).
void drawBell(int x, int y, uint16_t c) {
  frame.fillCircle(x + 7, y + 6, 5, c);
  frame.fillRect(x + 2, y + 6, 11, 5, c);
  frame.fillRect(x, y + 10, 15, 3, c);
  frame.fillCircle(x + 7, y + 15, 2, c);
}

// ============================================================================
// BATTERY (AXP2101)
// ----------------------------------------------------------------------------
// The AXP2101 power chip charges the LiPo from USB and reports battery
// voltage, charge %, charging and USB present over I2C.
// ============================================================================
// Registers as used by the XPowersLib driver: 0x00/0x01 status, 0x30 ADC
// enable, 0x34/0x35 battery mV, 0x68 battery detect, 0x18 fuel gauge,
// 0xA4 charge %. The % is estimated by the chip from voltage, so it can
// jump a little when USB is plugged in or unplugged.
bool batPresent = false, batCharging = false, usbIn = false;
int  batPct = -1, batMv = 0;
int  chargeMa = 400;                     // charge current, 200-1000 mA (command: charge)
                                         //   400 = about 0.5C for the 850 mAh cell
int  chargeMv = 4200;                    // charge-to voltage: 4100 or 4200 mV (command: chargev)
                                         //   4200 = full for a standard LiPo

// Charge current lives in the low 5 bits of register 0x62:
//   code 8 = 200 mA, then +100 mA per step up to code 16 = 1000 mA.
// chargeCode(): mA (200-1000, 100 mA steps) to the register code.
uint8_t chargeCode(int ma) { return 8 + (ma - 200) / 100; }
// chargeMaFromCode(): register code back to mA (codes below 8 are 25 mA steps).
int chargeMaFromCode(uint8_t c) { return c <= 8 ? c * 25 : 200 + (c - 8) * 100; }

// pmuSetBit(): Sets one bit of an AXP2101 register, only writing if it isn't set already.
void pmuSetBit(uint8_t reg, uint8_t bit) {
  uint8_t v;
  if (i2cRead(PMU_ADDR, reg, &v, 1) == 1 && !(v & (1 << bit))) i2cWrite(PMU_ADDR, reg, v | (1 << bit));
}

// chargeApply(): Writes chargeMa into the AXP2101 charge-current register (0x62) and
//   chargeMv into the charge-voltage register (0x64), leaving the other bits alone.
void chargeApply() {
  uint8_t v;
  if (i2cRead(PMU_ADDR, 0x62, &v, 1) != 1) return;
  uint8_t nv = (v & 0xE0) | chargeCode(chargeMa);
  if (nv != v) i2cWrite(PMU_ADDR, 0x62, nv);
  if (i2cRead(PMU_ADDR, 0x64, &v, 1) != 1) return;   // charge-to voltage
  nv = (v & 0xF8) | (chargeMv >= 4200 ? 3 : 2);
  if (nv != v) i2cWrite(PMU_ADDR, 0x64, nv);
}

// Charge-to voltage lives in the low 3 bits of register 0x64:
//   1 = 4.00 V, 2 = 4.10 V, 3 = 4.20 V, 4 = 4.35 V, 5 = 4.40 V.
// Only 4.10 / 4.20 are offered: 4.35 / 4.40 are for LiHV cells and would
// overcharge a normal LiPo.
// chargeReadMv(): The charge-to voltage the chip is set to, in mV (-1 = no answer / unknown).
int chargeReadMv() {
  static const int mv[] = { -1, 4000, 4100, 4200, 4350, 4400, -1, -1 };
  uint8_t v;
  if (i2cRead(PMU_ADDR, 0x64, &v, 1) != 1) return -1;
  return mv[v & 0x07];
}

// chargeReadMa(): The charge current the chip is actually set to, in mA (-1 = no answer).
int chargeReadMa() {
  uint8_t v;
  if (i2cRead(PMU_ADDR, 0x62, &v, 1) != 1) return -1;
  return chargeMaFromCode(v & 0x1F);
}

// batteryInit(): Enables what the battery readout needs in the AXP2101: battery detection,
//   the battery-voltage ADC and the fuel gauge (charge %).
void batteryInit() {
  pmuSetBit(0x68, 0);                    // battery detection on
  pmuSetBit(0x30, 0);                    // battery voltage measurement on
  pmuSetBit(0x18, 3);                    // fuel gauge on
  chargeApply();                         // saved charge current (charge command, default 400 mA)
}

// batteryRead(): Reads battery present / USB present / charging, the voltage and the charge %
//   from the AXP2101. Called every 5 s from loop().
void batteryRead() {
  uint8_t st[2], v[2], pct;
  if (i2cRead(PMU_ADDR, 0x00, st, 2) != 2) { batPresent = false; return; }
  batPresent  = st[0] & (1 << 3);
  usbIn       = (st[0] & (1 << 5)) && !(st[1] & (1 << 3));
  batCharging = ((st[1] >> 5) & 0x03) == 0x01;
  if (batPresent && i2cRead(PMU_ADDR, 0x34, v, 2) == 2) batMv = ((v[0] & 0x1F) << 8) | v[1];
  if (batPresent && i2cRead(PMU_ADDR, 0xA4, &pct, 1) == 1) batPct = min((int)pct, 100);
  if (!batPresent) { batPct = -1; batMv = 0; batCharging = false; }
}

// drawBolt(): Yellow lightning bolt (charging), two triangles.
void drawBolt(int x, int y, uint16_t c) {        // 10 x 16, top-left at x,y
  frame.fillTriangle(x + 6, y, x, y + 9, x + 5, y + 9, c);
  frame.fillTriangle(x + 4, y + 7, x + 10, y + 7, x + 3, y + 16, c);
}

// drawBattery(): Top-right battery icon: outline + nub, filled to the charge level
//   (red < 10%, yellow < 35%, green above), bolt to the left while charging.
//   bx = left edge of the body (274 upright, W - 46 sideways).
void drawBattery(int bx) {
  if (!batPresent) return;
  const int by = 13, bw = 34, bh = 18;   // body; nub on the right
  frame.drawRoundRect(bx, by, bw, bh, 4, C_LABEL);
  frame.drawRoundRect(bx + 1, by + 1, bw - 2, bh - 2, 3, C_LABEL);
  frame.fillRoundRect(bx + bw, by + 5, 3, bh - 10, 1, C_LABEL);
  int pct = batPct < 0 ? 0 : batPct;
  uint16_t c = pct < 10 ? C_BAD : (pct < 35 ? 0xFFE0 : C_OK);
  int fw = (bw - 6) * pct / 100;
  if (pct > 0 && fw < 2) fw = 2;
  if (fw > 0) frame.fillRect(bx + 3, by + 3, fw, bh - 6, c);
  if (batCharging) drawBolt(bx - 15, by + 1, 0xFFE0);
}

// alarm bell glyph (amber when the alarm is on, grey when off)
void drawAlarmBell(int cx, int cy, float sc, uint16_t c) {   // about 23 x 24 at sc 1
  auto S = [sc](float v) { return (int)lroundf(v * sc); };
  frame.fillCircle(cx, cy - S(11), max(1, S(2)), c);                       // knob
  frame.fillCircle(cx, cy - S(3), S(7), c);                                // dome
  frame.fillRect(cx - S(7), cy - S(3), S(15), S(9), c);                    // body
  frame.fillTriangle(cx - S(7), cy - S(1), cx - S(7), cy + S(6), cx - S(10), cy + S(6), c);   // flare
  frame.fillTriangle(cx + S(7), cy - S(1), cx + S(7), cy + S(6), cx + S(10), cy + S(6), c);
  frame.fillRoundRect(cx - S(11), cy + S(6), S(23), max(2, S(3)), 1, c);  // rim
  frame.fillCircle(cx, cy + S(11), max(1, S(2)), c);                       // clapper
}

// ---- Wi-Fi / Bluetooth status glyphs (face, tiles) ----
void drawWifiGlyph(int cx, int cy, float sc, uint16_t c) {   // ~26 x 22 at sc 1
  int by = cy + (int)(9 * sc);                               // fan point
  for (int i = 1; i <= 3; i++) {
    int ro = (int)(6 * i * sc), ri = ro - max(2, (int)(3 * sc));
    frame.fillArc(cx, by, ro, ri, 225, 315, c);
  }
  frame.fillCircle(cx, by - 1, max(2, (int)(2.5f * sc)), c);
}

// drawBtGlyph(): Bluetooth rune, drawn with wide lines; 'sc' scales it (1.0 on the face,
//   1.4 on the settings tile).
void drawBtGlyph(int cx, int cy, float sc, uint16_t c) {     // ~14 x 24 at sc 1
  float h = 11 * sc, w = 6 * sc, t = max(1.0f, 1.1f * sc);
  frame.drawWideLine(cx, cy - h, cx, cy + h, t, c);
  frame.drawWideLine(cx, cy - h, cx + w, cy - h / 2, t, c);
  frame.drawWideLine(cx + w, cy - h / 2, cx - w, cy + h / 2, t, c);
  frame.drawWideLine(cx, cy + h, cx + w, cy + h / 2, t, c);
  frame.drawWideLine(cx + w, cy + h / 2, cx - w, cy - h / 2, t, c);
}

// drawCross(): Red X over a status icon: radio on but not connected.
void drawCross(int cx, int cy, int r) {
  frame.drawWideLine(cx - r, cy - r, cx + r, cy + r, 1.6f, C_BAD);
  frame.drawWideLine(cx - r, cy + r, cx + r, cy - r, 1.6f, C_BAD);
}

// drawFlameGlyph(): Flame (heating), about 16 x 22 at sc 1: a round base with a
//   pointed tip, and a dark inner tongue.
void drawFlameGlyph(int cx, int cy, float sc, uint16_t c) {
  auto S = [sc](float v) { return (int)lroundf(v * sc); };
  frame.fillCircle(cx, cy + S(4), S(7), c);                                       // base
  frame.fillTriangle(cx - S(7), cy + S(3), cx + S(7), cy + S(3), cx + S(1), cy - S(11), c);   // tip
  frame.fillCircle(cx, cy + S(6), S(3), C_BG);                                    // inner tongue
  frame.fillTriangle(cx - S(3), cy + S(5), cx + S(3), cy + S(5), cx, cy - S(1), C_BG);
}

// drawSnowGlyph(): Snowflake (cooling), about 22 x 22 at sc 1: three crossed bars
//   with small V tips.
void drawSnowGlyph(int cx, int cy, float sc, uint16_t c) {
  float r = 10 * sc, t = max(1.0f, 1.2f * sc);
  for (int i = 0; i < 3; i++) {
    float a = i * PI / 3 + PI / 2;
    float dx = cosf(a) * r, dy = sinf(a) * r;
    frame.drawWideLine(cx - dx, cy - dy, cx + dx, cy + dy, t, c);
    for (int e = -1; e <= 1; e += 2) {                       // V tip at both ends
      float ex = cx + e * dx * 0.7f, ey = cy + e * dy * 0.7f;
      for (int k = -1; k <= 1; k += 2) {
        float b = a + k * 0.7f;
        frame.drawWideLine(ex, ey, ex + e * cosf(b) * r * 0.35f, ey + e * sinf(b) * r * 0.35f, t * 0.8f, c);
      }
    }
  }
}

// STATUS ICON GRID: every status icon is scaled so its longer side is 24 px
// (ICON_SC_*) and centred in a 24 x 24 cell; cells are ICON_STEP = 34 px
// apart across and down (a 10 px gap), starting at (22, 22).
const int ICON_STEP = 34, ICON_X0 = 22, ICON_Y0 = 22;
const float ICON_SC_WIFI = 0.94f, ICON_SC_BT = 1.09f, ICON_SC_NEST = 1.09f,
            ICON_SC_BELL = 0.92f, ICON_SC_NOTE = 1.04f;

// drawNestIcon(): Thermostat status under the Wi-Fi icon (nothing if the Nest
//   isn't set up):
//     orange flame      heating right now
//     blue snowflake    cooling right now
//     light grey        on (HEAT / COOL / HEAT-COOL) but idle - snowflake in
//                       COOL mode, flame otherwise
//     dark grey         thermostat switched OFF
//     red cross         can't reach the Nest (no data, or the last read failed)
void drawNestIcon(int cx, int cy) {
  if (!nestConfigured()) return;
  NestState n;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  n = nest;
  xSemaphoreGive(nestLock);
  bool ok = n.valid && !n.error[0];
  bool heating = ok && !strcmp(n.hvac, "HEATING"), cooling = ok && !strcmp(n.hvac, "COOLING");
  bool off = !strcmp(n.mode, "OFF");
  bool snow = cooling || (!heating && !strcmp(n.mode, "COOL"));
  uint16_t c = heating ? C_HEAT : cooling ? C_COOL : (!ok || off) ? C_RADIO_OFF : C_LABEL;
  if (snow) drawSnowGlyph(cx, cy, ICON_SC_NEST, c);
  else      drawFlameGlyph(cx, cy, ICON_SC_NEST, c);
  if (!ok) drawCross(cx, cy + 1, 10);
}

// drawNoteGlyph(): Music note (hourly chime), about 18 x 24 at sc 1: a slanted
//   oval head, a stem and a flag. hollow = outline only (quiet hours now).
void drawNoteGlyph(int cx, int cy, float sc, uint16_t c, bool hollow) {
  auto S = [sc](float v) { return lroundf(v * sc); };
  float t = max(1.5f, 2.2f * sc);
  int hx = cx - S(2), hy = cy + S(7);                                         // head centre
  frame.drawWideLine(cx + S(3), cy + S(6), cx + S(3), cy - S(12), t, c);     // stem
  frame.drawWideLine(cx + S(3), cy - S(12), cx + S(10), cy - S(5), t, c);    // flag
  frame.drawWideLine(cx + S(10), cy - S(5), cx + S(9), cy + S(1), t, c);
  if (hollow) {
    frame.drawEllipse(hx, hy, S(6), S(4), c);
    frame.drawEllipse(hx, hy, S(6) - 1, S(4) - 1, c);
  } else frame.fillEllipse(hx, hy, S(6), S(4), c);
}

bool inQuietHours();
// drawChimeIcon(): the hourly-chime status icon: amber note = chime on, hollow
//   grey note = quiet hours right now, nothing = chime off.
void drawChimeIcon(int cx, int cy, float sc) {
  if (!chimeOn) return;
  bool q = inQuietHours();
  drawNoteGlyph(cx - (int)lroundf(sc), cy + (int)lroundf(sc), sc, q ? C_DIM : C_HOUR, q);   // (glyph's centre is 1 px up-right of cx, cy)
}

// drawRadioIcons(): Wi-Fi, Bluetooth and Nest status in the top-left corner.
//   Wi-Fi / BT: grey = off, lit = on and connected, red cross = on but not connected
//   (for Bluetooth: no paired phone). The alarm bell is drawn with the date.
//   oneRow = true (digital face): the Nest icon goes in the top row too, right
//   of Bluetooth, instead of under Wi-Fi.
void drawRadioIcons(bool oneRow) {
  const int y = ICON_Y0, wx = ICON_X0, st = ICON_STEP;
  // where each icon goes (cell centres on the 34 px grid):
  //   digital    one row:        Wi-Fi, BT, Nest (then bell + chime: drawFaceBell)
  //   analog     one column:     Wi-Fi, BT, Nest, chime (upright only)
  //   jump hour  2 x 2 corner:   Wi-Fi, BT / Nest (chime next to the alarm bell: drawDate);
  //              a third icon in the top row would hit the dial's "15" label
  //   sideways   2 x 2 corner:   Wi-Fi, BT / Nest, chime
  bool column = (faceStyle == FACE_ANALOG && !isSideways());
  int btX = column ? wx : wx + st, btY = column ? y + st : y;
  int nX = oneRow ? wx + 2 * st : wx, nY = oneRow ? y : (column ? y + 2 * st : y + st);
  bool wConn = WiFi.status() == WL_CONNECTED;
  drawWifiGlyph(wx, y, ICON_SC_WIFI, wifiOn ? C_DATE : C_RADIO_OFF);
  if (wifiOn && !wConn) drawCross(wx, y + 1, 10);
  drawBtGlyph(btX, btY, ICON_SC_BT, btOn ? C_COOL : C_RADIO_OFF);
  if (btOn && !bleSecure) drawCross(btX, btY + 1, 10);
  drawNestIcon(nX, nY);
  if (column) drawChimeIcon(wx, y + (nestConfigured() ? 3 : 2) * st, ICON_SC_NOTE);   // no Nest: moves up
  else if (!oneRow && isSideways()) drawChimeIcon(wx + st, y + st, ICON_SC_NOTE);
}


// drawTopIcons(): Battery (top right) and the pull-down handle (top center).
//   (The status icons in the top-left corner are drawn by drawRadioIcons.)
void drawTopIcons() {
  drawBattery(274);
  frame.fillRoundRect(W / 2 - 22, 3, 44, 4, 2, 0x4208);   // pull-down handle: swipe down for settings
}

// ringBlink(): true / false alternating every 350 ms while the alarm rings -
//   the bell icons use it to flash (no pop-up any more).
bool ringBlink() { return (millis() / 350) % 2 == 0; }

// drawRinging(): For screens that have no bell of their own (sideways
//   weather, settings pages): a flashing amber bell at the top center.
//   The upright face and the sideways clock flash their own bell instead.
void drawRinging() {
  if (ringBlink()) drawAlarmBell(W / 2, 22, 1.2f, C_HOUR);
}

// ---- Nest panel ----
const Btn* findBtn(const Btn* list, int n, int id) {
  for (int i = 0; i < n; i++) if (list[i].id == id) return &list[i];
  return nullptr;
}

// drawArrowIn(): Filled up/down triangle centered in a button.
void drawArrowIn(const Btn* b, bool up, uint16_t c) {
  int cx = b->x + b->w / 2, cy = b->y + b->h / 2;
  if (up) frame.fillTriangle(cx - 16, cy + 9, cx + 16, cy + 9, cx, cy - 11, c);
  else    frame.fillTriangle(cx - 16, cy - 9, cx + 16, cy - 9, cx, cy + 11, c);
}

// drawNest(): Bottom-half Nest panel: big room temperature, humidity, heating/cooling
//   status, target with up/down arrows, and the five mode buttons. Reads a copy
//   of 'nest' taken under the lock so the network task can't change it mid-draw.
void drawNest() {
  NestState s;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  s = nest;
  xSemaphoreGive(nestLock);

  if (!nestConfigured() || !s.valid) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_center);
    frame.setTextColor(s.error[0] ? C_BAD : C_LABEL);
    const char* line1 = !nestConfigured() ? "Nest not set up" :
                        (WiFi.status() != WL_CONNECTED ? "waiting for Wi-Fi" :
                        (s.error[0] ? s.error : "reading thermostat..."));
    frame.drawString(line1, 160, 340);
    if (!nestConfigured()) {
      frame.setFont(&fonts::DejaVu12);
      frame.setTextColor(C_DIM);
      frame.drawString("run nest_auth.py on the Mac", 160, 372);
    }
    return;
  }

  bool f = s.scaleF;
  bool eco = !strcmp(s.eco, "MANUAL_ECO");
  // HVAC status, top right of the panel
  frame.setFont(&fonts::DejaVu18);
  frame.setTextDatum(top_right);
  if (s.error[0] && millis() - s.updatedMs > 180000UL) { frame.setTextColor(C_BAD); frame.drawString("offline", W - 10, NEST_Y); }
  else if (!strcmp(s.hvac, "HEATING")) { frame.setTextColor(C_HEAT); frame.drawString("Heating", W - 10, NEST_Y); }
  else if (!strcmp(s.hvac, "COOLING")) { frame.setTextColor(C_COOL); frame.drawString("Cooling", W - 10, NEST_Y); }
  else { frame.setTextColor(C_DIM); frame.drawString("Idle", W - 10, NEST_Y); }

  // room temperature, big, left
  frame.setTextDatum(middle_center);
  frame.setFont(&fonts::DejaVu72);
  frame.setTextColor(TFT_WHITE);
  String amb = fmtTemp(s.ambientC, f);
  frame.drawString(amb, 100, 340);
  int degX = 100 + frame.textWidth(amb) / 2 + 2;     // (still in the big font)
  frame.setFont(&fonts::DejaVu24);
  frame.setTextDatum(top_left);
  frame.drawString(f ? "F" : "C", degX, 306);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextDatum(middle_center);
  frame.setTextColor(C_DIM);
  char hum[24];
  if (s.humidity >= 0) snprintf(hum, sizeof(hum), "INSIDE  -  %d%% humidity", s.humidity);
  else snprintf(hum, sizeof(hum), "INSIDE");
  frame.drawString(hum, 100, 396);

  // target, right: arrows + value (amber while waiting to be sent)
  bool canSet = !eco && (!strcmp(s.mode, "HEAT") || !strcmp(s.mode, "COOL") || !strcmp(s.mode, "HEATCOOL"));
  const Btn* up = findBtn(NBTNS, NNB, B_N_UP);
  const Btn* dn = findBtn(NBTNS, NNB, B_N_DN);
  uint16_t ac = canSet ? (pressedBtn == B_N_UP ? C_HOUR : C_LABEL) : 0x2945;
  drawArrowIn(up, true, ac);
  ac = canSet ? (pressedBtn == B_N_DN ? C_HOUR : C_LABEL) : 0x2945;
  drawArrowIn(dn, false, ac);
  bool usePend = pendingSet || (sentShow && nestVersion == sentVersion);
  float hC = usePend ? pendHeatC : s.heatC;
  float cC = usePend ? pendCoolC : s.coolC;
  String tgt;
  uint16_t tc = pendingSet ? C_HOUR : TFT_WHITE;
  if (eco)                              tgt = "ECO";
  else if (!strcmp(s.mode, "HEAT"))     { tgt = fmtTemp(hC, f); tc = pendingSet ? C_HOUR : C_HEAT; }
  else if (!strcmp(s.mode, "COOL"))     { tgt = fmtTemp(cC, f); tc = pendingSet ? C_HOUR : C_COOL; }
  else if (!strcmp(s.mode, "HEATCOOL")) tgt = fmtTemp(hC, f) + "-" + fmtTemp(cC, f);
  else                                  { tgt = "OFF"; tc = C_DIM; }
  frame.setTextDatum(middle_center);
  frame.setFont(tgt.length() > 4 ? &fonts::DejaVu24 : &fonts::DejaVu40);
  frame.setTextColor(tc);
  frame.drawString(tgt, 266, 354);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.drawString("SET TO", 266, 284);

  // mode buttons
  const char* labels[5] = { "HEAT", "COOL", "AUTO", "OFF", "ECO" };
  const int ids[5] = { B_M_HEAT, B_M_COOL, B_M_AUTO, B_M_OFF, B_M_ECO };
  for (int i = 0; i < 5; i++) {
    const Btn* b = findBtn(NBTNS, NNB, ids[i]);
    bool active = (i == 0 && !eco && !strcmp(s.mode, "HEAT")) ||
                  (i == 1 && !eco && !strcmp(s.mode, "COOL")) ||
                  (i == 2 && !eco && !strcmp(s.mode, "HEATCOOL")) ||
                  (i == 3 && !eco && !strcmp(s.mode, "OFF")) ||
                  (i == 4 && eco);
    uint16_t fill = active ? (i == 0 ? C_HEAT : i == 1 ? C_COOL : i == 2 ? 0x9CDF : i == 4 ? C_OK : 0x8410) : C_PANEL;
    if (pressedBtn == ids[i]) fill = C_HOUR;
    frame.fillRoundRect(b->x, b->y, b->w, b->h, 9, fill);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(active || pressedBtn == ids[i] ? TFT_BLACK : C_LABEL);
    frame.setTextDatum(middle_center);
    frame.drawString(labels[i], b->x + b->w / 2, b->y + b->h / 2 + 1);
  }

  // a short note under the panel header (command sent / refused ...)
  if (millis() < nestMsgUntil) {
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_HOUR);
    frame.setTextDatum(top_center);
    frame.drawString(nestMsg, 160, 414);
  } else if (s.error[0]) {
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_BAD);
    frame.setTextDatum(top_center);
    frame.drawString(s.error, 160, 414);
  }
}

// ---- weather panel ----
const uint16_t C_SUN = 0xFE60, C_MOON = 0xE71C, C_CLOUD = 0xC618, C_CLOUD_D = 0x7BEF, C_BOLT = 0xFFE0;

// drawCloudShape(): Cloud made of three circles and a rounded base, 'w' pixels wide.
void drawCloudShape(int cx, int cy, int w, uint16_t c) {   // w = width
  frame.fillCircle(cx - w * 22 / 100, cy, w * 20 / 100, c);
  frame.fillCircle(cx + w * 4 / 100, cy - w * 10 / 100, w * 28 / 100, c);
  frame.fillCircle(cx + w * 28 / 100, cy + w * 2 / 100, w * 19 / 100, c);
  frame.fillRoundRect(cx - w * 45 / 100, cy, w * 92 / 100, w * 21 / 100, w * 10 / 100, c);
}

// drawSunShape(): Sun disc with 8 rays.
void drawSunShape(int cx, int cy, int r) {
  for (int i = 0; i < 8; i++) {
    float a = i * PI / 4;
    frame.drawWideLine(cx + (r + 3) * cosf(a), cy + (r + 3) * sinf(a),
                       cx + (r + 3 + r * 6 / 10) * cosf(a), cy + (r + 3 + r * 6 / 10) * sinf(a),
                       max(1.0f, r / 7.0f), C_SUN);
  }
  frame.fillCircle(cx, cy, r, C_SUN);
}

// drawMoonShape(): Crescent: a disc with a background-colored disc bitten out of it.
void drawMoonShape(int cx, int cy, int r) {
  frame.fillCircle(cx, cy, r, C_MOON);
  frame.fillCircle(cx + r * 6 / 10, cy - r * 4 / 10, r * 9 / 10, C_BG);
}

// wxGroup(): Collapses the WMO weather codes into 8 icon groups
//   (clear, partly cloudy, overcast, fog, drizzle, rain, snow, thunder).
int wxGroup(int c) {
  if (c == 0) return 0;                          // clear
  if (c == 1 || c == 2) return 1;                // partly cloudy
  if (c == 3) return 2;                          // overcast
  if (c == 45 || c == 48) return 3;              // fog
  if (c >= 51 && c <= 57) return 4;              // drizzle
  if ((c >= 61 && c <= 67) || (c >= 80 && c <= 82)) return 5;   // rain
  if ((c >= 71 && c <= 77) || c == 85 || c == 86) return 6;    // snow
  if (c >= 95) return 7;                         // thunder
  return 2;
}

// wxText(): Short description for a WMO weather code ('Partly cloudy', 'Showers'...).
const char* wxText(int c) {
  switch (c) {
    case 0: return "Clear";            case 1: return "Mostly clear";
    case 2: return "Partly cloudy";    case 3: return "Overcast";
    case 45: case 48: return "Fog";
    case 51: case 53: case 55: return "Drizzle";
    case 56: case 57: return "Freezing drizzle";
    case 61: return "Light rain";      case 63: return "Rain";
    case 65: return "Heavy rain";      case 66: case 67: return "Freezing rain";
    case 71: return "Light snow";      case 73: return "Snow";
    case 75: return "Heavy snow";      case 77: return "Snow grains";
    case 80: case 81: case 82: return "Showers";
    case 85: case 86: return "Snow showers";
    case 95: return "Thunderstorm";
    case 96: case 99: return "Storm, hail";
  }
  return "";
}

// weather icon about w pixels wide, centered on cx, cy
void drawWxIcon(int cx, int cy, int w, int code, bool day) {
  int g = wxGroup(code);
  if (g == 0) { if (day) drawSunShape(cx, cy, w * 24 / 100); else drawMoonShape(cx, cy, w * 30 / 100); return; }
  if (g == 1) {
    if (day) drawSunShape(cx + w * 16 / 100, cy - w * 14 / 100, w * 17 / 100);
    else drawMoonShape(cx + w * 16 / 100, cy - w * 14 / 100, w * 20 / 100);
    drawCloudShape(cx - w * 6 / 100, cy + w * 8 / 100, w * 75 / 100, C_CLOUD);
    return;
  }
  if (g == 3) {
    for (int i = 0; i < 4; i++)
      frame.fillRoundRect(cx - w * 40 / 100 + (i % 2) * w / 10, cy - w * 24 / 100 + i * w * 15 / 100,
                          w * 70 / 100, max(2, w / 14), 2, C_CLOUD);
    return;
  }
  int top = (g == 2) ? cy : cy - w * 12 / 100;
  drawCloudShape(cx, top, (g == 2) ? w * 90 / 100 : w * 85 / 100, g == 7 ? C_CLOUD_D : C_CLOUD);
  int by = top + w * 30 / 100;                   // below the cloud
  if (g == 4 || g == 5) {
    int drops = (g == 4) ? 2 : 3;
    for (int i = 0; i < drops; i++) {
      int x = cx - w * 20 / 100 + i * w * 20 / 100 + (g == 4 ? w / 10 : 0);
      frame.drawWideLine(x, by, x - w * 5 / 100, by + w * 16 / 100, max(1.0f, w / 30.0f), C_COOL);
    }
  } else if (g == 6) {
    for (int i = 0; i < 3; i++) frame.fillCircle(cx - w * 20 / 100 + i * w * 20 / 100, by + (i % 2) * w / 12 + w / 20, max(2, w / 16), TFT_WHITE);
  } else if (g == 7) {
    int x = cx, y = by - w / 20;
    frame.fillTriangle(x + w / 10, y, x - w / 12, y + w * 14 / 100, x + w / 30, y + w * 14 / 100, C_BOLT);
    frame.fillTriangle(x + w / 30, y + w * 10 / 100, x + w / 8, y + w * 10 / 100, x - w / 14, y + w * 28 / 100, C_BOLT);
  }
}

// panelUnitsF(): Weather uses the thermostat's unit if the Nest has been read, else Fahrenheit.
bool panelUnitsF() {                      // follow the thermostat's unit, else F
  bool f = true;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  if (nest.valid) f = nest.scaleF;
  xSemaphoreGive(nestLock);
  return f;
}

// drawDegree(): Degree sign as a small double circle (the fonts have no degree glyph).
void drawDegree(int x, int y, int r, uint16_t c) { frame.drawCircle(x, y, r, c); frame.drawCircle(x, y, r - 1, c); }

// drawRainChance(): Blue raindrop + 'NN%' (precipitation probability).
void drawRainChance(int cx, int cy, int pct) {
  frame.fillCircle(cx - 16, cy + 2, 3, C_COOL);
  frame.fillTriangle(cx - 19, cy + 1, cx - 13, cy + 1, cx - 16, cy - 5, C_COOL);
  char b[8];
  snprintf(b, sizeof(b), "%d%%", pct);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_COOL);
  frame.setTextDatum(middle_left);
  frame.drawString(b, cx - 9, cy);
}

const char* WDAY_SHORT[7] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

// drawWeather(): Bottom-half weather panel (portrait): now (icon, big temperature, conditions,
//   humidity, wind) and three columns for today + 2 days.
void drawWeather() {
  WxState w;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  w = wx;
  xSemaphoreGive(nestLock);
  bool f = panelUnitsF();

  if (wxHaveLoc) {
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(top_right);
    frame.drawString(fitText(wxPlace, 190, false), W - 10, NEST_Y + 4);
  }
  if (!wxHaveLoc || !w.valid) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_center);
    frame.setTextColor(w.error[0] ? C_BAD : C_LABEL);
    const char* line1 = !wxHaveLoc ? "Set your city" :
                        (WiFi.status() != WL_CONNECTED ? "waiting for Wi-Fi" :
                        (w.error[0] ? w.error : "getting the forecast..."));
    frame.drawString(line1, 160, 340);
    if (!wxHaveLoc) {
      frame.setFont(&fonts::DejaVu12);
      frame.setTextColor(C_DIM);
      frame.drawString("Serial Monitor:  city Portland, OR", 160, 372);
    }
    return;
  }

  // now
  drawWxIcon(52, 316, 64, w.curCode, w.isDay);
  int t = (int)lroundf(cToShown(w.curC, f));
  char b[16];
  snprintf(b, sizeof(b), "%d", t);
  frame.setFont(&fonts::DejaVu56);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(b, 146, 318);
  int tw = frame.textWidth(b);
  drawDegree(146 + tw / 2 + 7, 298, 5, TFT_WHITE);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextDatum(middle_left);
  frame.setTextColor(TFT_WHITE);
  frame.drawString(fitText(wxText(w.curCode), 104, false), 210, 300);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  if (w.curHum >= 0) { snprintf(b, sizeof(b), "humidity %d%%", w.curHum); frame.drawString(b, 210, 324); }
  if (f) snprintf(b, sizeof(b), "wind %d mph", (int)lroundf(w.windKmh * 0.6214f));
  else   snprintf(b, sizeof(b), "wind %d km/h", (int)lroundf(w.windKmh));
  frame.drawString(b, 210, 342);

  // next days
  frame.drawFastHLine(20, 360, W - 40, 0x2104);
  for (int i = 0; i < w.n && i < 3; i++) {   // portrait: today + 2 days
    int cx = 56 + i * 104;
    if (i) frame.drawFastVLine(cx - 52, 370, 100, 0x2104);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_center);
    frame.setTextColor(i == 0 ? C_HOUR : C_LABEL);
    frame.drawString(i == 0 ? "TODAY" : WDAY_SHORT[w.wday[i]], cx, 377);
    drawWxIcon(cx, 408, 40, w.code[i], true);
    int hi = (int)lroundf(cToShown(w.hiC[i], f)), lo = (int)lroundf(cToShown(w.loC[i], f));
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(TFT_WHITE);
    frame.setTextDatum(middle_right);
    snprintf(b, sizeof(b), "%d", hi);
    frame.drawString(b, cx - 4, 442);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(middle_left);
    snprintf(b, sizeof(b), "%d", lo);
    frame.drawString(b, cx + 4, 442);
    frame.drawFastVLine(cx, 434, 16, 0x4208);
    if (w.pop[i] >= 0) drawRainChance(cx, 465, w.pop[i]);
  }
}

// the pill at the top left of the bottom panel: shows which panel, tap to switch
const int PILL_X = 8, PILL_Y = 252, PILL_W = 100, PILL_H = 24;

// drawPanelPill(): The WEATHER / NEST pill at the top left of the bottom half (tap to switch).
void drawPanelPill() {
  bool pressed = (pressedBtn == -1);
  frame.fillRoundRect(PILL_X, PILL_Y, PILL_W, PILL_H, PILL_H / 2, pressed ? C_HOUR : C_PANEL);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(pressed ? TFT_BLACK : C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString(panelMode == 1 ? "WEATHER" : "NEST", PILL_X + 12, PILL_Y + PILL_H / 2 + 1);
  int ax = PILL_X + PILL_W - 16, ay = PILL_Y + PILL_H / 2;      // swap arrows
  uint16_t c = pressed ? TFT_BLACK : C_DIM;
  frame.fillTriangle(ax - 4, ay - 2, ax + 4, ay - 2, ax, ay - 7, c);
  frame.fillTriangle(ax - 4, ay + 2, ax + 4, ay + 2, ax, ay + 7, c);
}

// drawPanel(): Bottom half: divider line, pill, then the weather or Nest panel.
void drawPanel() {
  frame.drawFastHLine(10, NEST_Y - 4, W - 20, C_ARC);
  drawPanelPill();
  if (panelMode == 1) drawWeather();
  else drawNest();
}

// ============================================================================
// ANALOG AND DIGITAL FACES
// ----------------------------------------------------------------------------
// The two other clock faces (Settings > Clock > FACE). Each draws at any size
// from a centre and a scale, so the same code makes the upright face (top
// half) and the big sideways clock. The corners - status icons, battery,
// date, alarm bell - are drawn around them exactly as for the jump face.
// ============================================================================

// facePt(): point at 'deg' (0 = 12 o'clock, clockwise) and radius r from cx, cy.
void facePt(int cx, int cy, float deg, float r, float& x, float& y) {
  float a = deg * (PI / 180.0f);
  x = cx + r * sinf(a);
  y = cy - r * cosf(a);
}

// drawFaceHand(): A tapered hand from a short tail behind the centre out to
//   'len': 'w0' wide at the hub, 'w1' at the tip.
void drawFaceHand(int cx, int cy, float deg, float len, float tail, float w0, float w1, uint16_t c) {
  float x0, y0, x1, y1;
  facePt(cx, cy, deg + 180, tail, x0, y0);
  facePt(cx, cy, deg, len, x1, y1);
  frame.drawWedgeLine(x0, y0, x1, y1, w0 / 2, w1 / 2, c);
}

// drawAnalogFace(): The analog face, same look as the round GC9A01 Desk Pet's
//   analog clock (Gadgets/DeskPet_Round), without its date window (this face
//   has the date in the corners): 60 ticks - every 5th longer and white, the
//   rest dark grey - no outer ring, bold 12 / 3 / 6 / 9, amber hour hand,
//   white minute hand, cyan seconds hand with a long tail, cyan centre cap.
//   r = the ticks' outer radius; the round design is 113, so k scales it.
void drawAnalogFace(int cx, int cy, int r, const struct tm& t, bool known) {
  const uint16_t AMBER = frame.color565(255, 190, 60), CYAN = frame.color565(80, 220, 255),
                 WHITE = frame.color565(240, 240, 240), DIM = frame.color565(70, 70, 70);
  float k = r / 113.0f;
  for (int m = 0; m < 60; m++) {         // ticks: hours bigger and white
    float x0, y0, x1, y1;
    bool big = m % 5 == 0;
    facePt(cx, cy, m * 6, (big ? 100 : 106) * k, x0, y0);
    facePt(cx, cy, m * 6, r, x1, y1);
    frame.drawWideLine(x0, y0, x1, y1, (big ? 2.0f : 1.0f) * k, big ? WHITE : DIM);   // (half-width)
  }
  frame.setFont(k > 1.1f ? &fonts::FreeSansBold18pt7b : &fonts::FreeSansBold12pt7b);
  frame.setTextColor(WHITE);
  frame.setTextDatum(middle_center);
  int n = (int)lroundf(84 * k);
  frame.drawString("12", cx, cy - n);
  frame.drawString("3", cx + n, cy + 1);
  frame.drawString("6", cx, cy + n + 1);
  frame.drawString("9", cx - n, cy + 1);
  if (known) {
    float sec = t.tm_sec, mins = t.tm_min + sec / 60.0f, hr = (t.tm_hour % 12) + mins / 60.0f;
    // hands: angle, length, tail behind the centre, width - as on the round display
    drawFaceHand(cx, cy, hr * 30,  55 * k, 10 * k, 9 * k, 9 * k, AMBER);
    drawFaceHand(cx, cy, mins * 6, 82 * k, 12 * k, 6 * k, 6 * k, WHITE);
    drawFaceHand(cx, cy, sec * 6,  96 * k, 22 * k, 2.5f * k, 2.5f * k, CYAN);
  }
  frame.fillSmoothCircle(cx, cy, 7 * k, CYAN);   // the centre cap
  frame.fillSmoothCircle(cx, cy, 3 * k, C_BG);
}

// drawDigitalFace(): The Kindle Home Hub's digital face: big bold 24-hour
//   HH:MM in amber with the seconds beside it (smaller, cyan), a thin amber
//   rule, the weekday in white and the date in sky blue - the jump face's
//   colours. The weekday is bold; all text is anti-aliased DejaVu Sans
//   (clock_fonts.h). 'land' picks the sideways sizes. This face shows its own
//   date, so the corner date isn't drawn with it; all the status icons and
//   the alarm bell sit in one row along the top instead.
void drawDigitalFace(bool land, const struct tm& t, bool known) {
  // layout: centre x, time baseline, rule, weekday + date baselines
  const int cx    = W / 2;
  // (the block is centred between the icon row and the bottom edge / panel)
  const int base  = land ? 166 : 128;
  const int ruleY = land ? 188 : 146;
  const int ruleH = land ? 170 : 128;    // half length of the rule
  const int wdY   = land ? 234 : 182;
  const int dtY   = land ? 274 : 212;
  const int gap   = land ? 12 : 8;       // between HH:MM and the seconds

  char hm[8], ss[4];
  if (known) {
    snprintf(hm, sizeof(hm), "%02d:%02d", t.tm_hour, t.tm_min);
    snprintf(ss, sizeof(ss), "%02d", t.tm_sec);
  } else { strcpy(hm, "--:--"); strcpy(ss, "--"); }

  // widths first, to centre HH:MM + seconds together ("88" keeps it from
  // shifting as the digits change)
  frame.loadFont(land ? VLW_SEC_L : VLW_SEC_P);
  int sw = frame.textWidth("88");
  frame.unloadFont();
  frame.loadFont(land ? VLW_TIME_L : VLW_TIME_P);
  int tw = frame.textWidth("88:88");
  int x0 = cx - (tw + gap + sw) / 2;
  frame.setTextColor(C_HOUR);             // time: amber
  frame.setTextDatum(baseline_right);    // right-aligned: the digits stay put
  frame.drawString(hm, x0 + tw, base);
  frame.unloadFont();

  frame.loadFont(land ? VLW_SEC_L : VLW_SEC_P);  // seconds: smaller, cyan, same baseline
  frame.setTextColor(C_SECON);
  frame.setTextDatum(baseline_left);
  frame.drawString(ss, x0 + tw + gap, base);
  frame.unloadFont();

  frame.loadFont(land ? VLW_BOLD_L : VLW_BOLD_P);
  frame.drawFastHLine(cx - ruleH, ruleY, 2 * ruleH, 0x7A60);       // dark amber rule
  frame.drawFastHLine(cx - ruleH, ruleY + 1, 2 * ruleH, 0x7A60);

  static const char* DAYS[7] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
  static const char* MONTHS[12] = { "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
                                    "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER" };
  frame.setTextColor(TFT_WHITE);          // weekday: white
  frame.setTextDatum(baseline_center);
  frame.drawString(known ? DAYS[t.tm_wday] : "---", cx, wdY);
  frame.unloadFont();

  frame.loadFont(land ? VLW_DATE_L : VLW_DATE_P);
  char db[32];
  if (known) snprintf(db, sizeof(db), "%s %d, %d", MONTHS[t.tm_mon], t.tm_mday, t.tm_year + 1900);
  else strcpy(db, "-");
  frame.setTextColor(C_DATE);             // date: sky blue
  frame.setTextDatum(baseline_center);
  frame.drawString(db, cx, dtY);
  frame.unloadFont();
  frame.setTextDatum(top_left);
}

// drawFaceBell(): The alarm bell for the digital face, last in the top row of
//   icons (after Wi-Fi, Bluetooth and the Nest): amber = on, flashing while it
//   rings, nothing when the alarm is off - then the chime note moves up into
//   its place.
void drawFaceBell() {
  int bx = ICON_X0 + (nestConfigured() ? 3 : 2) * ICON_STEP;   // (no Nest icon: take its place)
  bool bell = alarmOn || alarmRinging;
  if (alarmRinging) { if (ringBlink()) drawAlarmBell(bx, ICON_Y0, 1.2f * ICON_SC_BELL, C_HOUR); }
  else if (alarmOn) drawAlarmBell(bx, ICON_Y0, ICON_SC_BELL, C_HOUR);
  drawChimeIcon(bell ? bx + ICON_STEP : bx, ICON_Y0, ICON_SC_NOTE);   // the hourly chime's note, last in the row
}

// ============================================================================
// SIDEWAYS SCREENS (480 X 320)
// ----------------------------------------------------------------------------
// The two landscape layouts chosen by the motion sensor: turned left = the
// big clock alone, turned right = a full-screen forecast.
// ============================================================================
void drawDateLandscape(const struct tm& t, bool known) {   // as in WS35_JumpHour
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

// renderLandscapeClock(): Sideways (turned left): the big clock alone, with
//   the date, Wi-Fi / Bluetooth top left, battery top right, and a small bell
//   next to the radio icons when the alarm is on (flashing while it rings).
//   The corners are outside the dial's labels, so nothing overlaps.
void renderLandscapeClock(float handDeg, int sec, int hFrom, int hTo, float roll, bool known, const struct tm& t) {
  frame.fillScreen(C_BG);
  if (faceStyle == FACE_ANALOG) drawAnalogFace(W / 2, H / 2, 140, t, known);
  else if (faceStyle == FACE_DIGITAL) drawDigitalFace(true, t, known);
  else {
    drawFan();
    drawSeconds(known ? sec : -1);
    drawHand(handDeg);
    drawHourWindow(hFrom, hTo, roll, known);
  }
  if (faceStyle != FACE_DIGITAL) drawDateLandscape(t, known);   // (digital shows its own date)
  drawRadioIcons(faceStyle == FACE_DIGITAL);   // same top-left spot as upright
  drawBattery(W - 46);                   // top right
  if (faceStyle == FACE_DIGITAL) drawFaceBell();                              // last in the icon row
  else if (alarmRinging) { if (ringBlink()) drawAlarmBell(ICON_X0 + 2 * ICON_STEP, ICON_Y0, 1.2f * ICON_SC_BELL, C_HOUR); }
  else if (alarmOn) drawAlarmBell(ICON_X0 + 2 * ICON_STEP, ICON_Y0, ICON_SC_BELL, C_HOUR);   // alarm set
  frame.pushSprite(0, 0);
}

// renderLandscapeWeather(): Sideways (turned right): full-screen forecast - place and time on top,
//   current conditions, and seven day columns.
void renderLandscapeWeather(const struct tm& t, bool known) {
  frame.fillScreen(C_BG);
  WxState w;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  w = wx;
  xSemaphoreGive(nestLock);
  bool f = panelUnitsF();
  char b[24];

  // top bar: place left, time and weekday right
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_left);
  frame.drawString(wxHaveLoc ? fitText(wxPlace, 190, false) : String("Weather"), 16, 20);
  if (known) {
    snprintf(b, sizeof(b), "%02d:%02d", t.tm_hour, t.tm_min);
    frame.setFont(&fonts::DejaVu24);
    frame.setTextColor(C_HOUR);
    frame.setTextDatum(middle_right);
    frame.drawString(b, W - 16, 20);
    int tw = frame.textWidth(b);           // (still in the 24 px font)
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(C_DATE);
    snprintf(b, sizeof(b), "%s %d", WD[t.tm_wday], t.tm_mday);
    frame.drawString(b, W - 26 - tw, 21);
  }
  frame.drawFastHLine(16, 40, W - 32, 0x2104);

  if (!wxHaveLoc || !w.valid) {
    frame.setFont(&fonts::DejaVu24);
    frame.setTextDatum(middle_center);
    frame.setTextColor(w.error[0] ? C_BAD : C_LABEL);
    frame.drawString(!wxHaveLoc ? "Set your ZIP: Settings > Weather" :
                     (WiFi.status() != WL_CONNECTED ? "waiting for Wi-Fi" :
                     (w.error[0] ? w.error : "getting the forecast...")), W / 2, H / 2);
    if (alarmRinging) drawRinging();
    frame.pushSprite(0, 0);
    return;
  }

  // now: icon, big temperature, conditions
  drawWxIcon(72, 98, 96, w.curCode, w.isDay);
  snprintf(b, sizeof(b), "%d", (int)lroundf(cToShown(w.curC, f)));
  frame.setFont(&fonts::DejaVu72);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(b, 200, 100);
  int tw = frame.textWidth(b);
  drawDegree(200 + tw / 2 + 9, 74, 6, TFT_WHITE);
  frame.setTextDatum(middle_left);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.drawString(fitText(wxText(w.curCode), 166, false), 298, 68);
  if (w.n > 0) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(TFT_WHITE);
    snprintf(b, sizeof(b), "H %d", (int)lroundf(cToShown(w.hiC[0], f)));
    frame.drawString(b, 298, 98);
    frame.setTextColor(C_DIM);
    snprintf(b, sizeof(b), "L %d", (int)lroundf(cToShown(w.loC[0], f)));
    frame.drawString(b, 362, 98);
  }
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  if (w.curHum >= 0) { snprintf(b, sizeof(b), "humidity %d%%", w.curHum); frame.drawString(b, 298, 124); }
  if (f) snprintf(b, sizeof(b), "wind %d mph", (int)lroundf(w.windKmh * 0.6214f));
  else   snprintf(b, sizeof(b), "wind %d km/h", (int)lroundf(w.windKmh));
  frame.drawString(b, 298, 142);

  // 7 days
  frame.drawFastHLine(16, 160, W - 32, 0x2104);
  int n = min(w.n, 7);
  const int colW = 66, x0 = (W - colW * 7) / 2;
  for (int i = 0; i < n; i++) {
    int cx = x0 + colW * i + colW / 2;
    if (i) frame.drawFastVLine(cx - colW / 2, 172, 136, 0x2104);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_center);
    frame.setTextColor(i == 0 ? C_HOUR : C_LABEL);
    frame.drawString(i == 0 ? "TOD" : WDAY_SHORT[w.wday[i]], cx, 184);
    drawWxIcon(cx, 222, 46, w.code[i], true);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(TFT_WHITE);
    snprintf(b, sizeof(b), "%d", (int)lroundf(cToShown(w.hiC[i], f)));
    frame.drawString(b, cx, 260);
    frame.setTextColor(C_DIM);
    snprintf(b, sizeof(b), "%d", (int)lroundf(cToShown(w.loC[i], f)));
    frame.drawString(b, cx, 282);
    if (w.pop[i] >= 0) drawRainChance(cx + 4, 304, w.pop[i]);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// render(): Draws one complete frame into the PSRAM sprite and pushes it to the display
//   (about 30 ms with DMA). Everything on the face goes through here, so there
//   is never any flicker. Picks the layout for the current orientation.
void render(float handDeg, int sec, int hFrom, int hTo, float roll, bool known, const struct tm& t) {
  if (orient == 2) { renderLandscapeClock(handDeg, sec, hFrom, hTo, roll, known, t); return; }
  if (orient == 1) { renderLandscapeWeather(t, known); return; }
  frame.fillScreen(C_BG);
  if (faceStyle == FACE_ANALOG) {
    drawRadioIcons(false);
    drawAnalogFace(160, 124, 104, t, known);
  } else if (faceStyle == FACE_DIGITAL) {
    drawRadioIcons(true);                // Wi-Fi, BT, Nest, bell: all in the top row
    drawDigitalFace(false, t, known);
    drawFaceBell();
  } else {
    drawFan();
    drawSeconds(known ? sec : -1);
    drawRadioIcons(false);
    drawHand(handDeg);
    drawHourWindow(hFrom, hTo, roll, known);
  }
  if (faceStyle != FACE_DIGITAL) drawDate(t, known);   // (digital shows its own date)
  drawTopIcons();
  drawPanel();
  if (!holdPush) frame.pushSprite(0, 0);
}

// ============================================================================
// SETTINGS SCREEN
// ----------------------------------------------------------------------------
// Settings > Clock page (time, alarm, volume, alarm sound) and the button
// helpers it uses. Each page has its own list of buttons (Btn: id, x, y,
// w, h); hitTest() finds which one a touch landed on.
// ============================================================================
int hitTest(const Btn* list, int n, int x, int y) {
  for (int i = 0; i < n; i++) {
    const Btn& b = list[i];
    if (x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h) return b.id;
  }
  return B_NONE;
}

// drawSButton(): Rounded button from the Clock page's button list (SBTNS), text centered.
void drawSButton(int id, const char* label, uint16_t fill, uint16_t text) {
  const Btn* b = findBtn(SBTNS, NSB, id);
  frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, fill);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(text);
  frame.setTextDatum(middle_center);
  frame.drawString(label, b->x + b->w / 2, b->y + b->h / 2 + 1);
}

// drawSArrow(): Up/down arrow button on the Clock page; amber while held.
void drawSArrow(int id, bool up) {
  drawArrowIn(findBtn(SBTNS, NSB, id), up, pressedBtn == id ? C_HOUR : C_LABEL);
}

// drawFields(): Big HH : MM pair used by the TIME and ALARM panels.
void drawFields(int yc, int h, int m, uint16_t c) {
  char b[4];
  frame.setFont(&fonts::DejaVu56);
  frame.setTextColor(c);
  frame.setTextDatum(middle_center);
  snprintf(b, sizeof(b), "%02d", h); frame.drawString(b, 80, yc);
  frame.drawString(":", 125, yc - 4);
  snprintf(b, sizeof(b), "%02d", m); frame.drawString(b, 170, yc);
}

// renderSettings(): The Clock settings page: FACE, TIME panel (arrows, SET, Wi-Fi SYNC)
//   and ALARM panel (arrows, ON/OFF, volume, tappable sound name).
void renderSettings() {
  frame.fillScreen(C_BG);
  drawSButton(B_BACK, "< BACK", C_DATE, TFT_BLACK);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString("Clock", 160, 30);
  {                                      // FACE: tap for the next clock face (two-line label)
    const Btn* b = findBtn(SBTNS, NSB, B_FACE);
    frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, 0x4208);
    frame.setTextDatum(middle_center);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_LABEL);
    frame.drawString("FACE", b->x + b->w / 2, b->y + 12);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(C_HOUR);
    frame.drawString(FACE_NAMES[faceStyle], b->x + b->w / 2, b->y + 31);
  }

  frame.drawRoundRect(10, 60, 300, 190, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("TIME", 22, 74);
  drawSArrow(B_TH_UP, true);  drawSArrow(B_TH_DN, false);
  drawSArrow(B_TM_UP, true);  drawSArrow(B_TM_DN, false);
  drawFields(158, editH, editM, TFT_WHITE);
  drawSButton(B_SET, "SET", 0x4208, TFT_WHITE);
  {                                      // Wi-Fi SYNC: two-line label
    const Btn* b = findBtn(SBTNS, NSB, B_WSYNC);
    bool p = pressedBtn == B_WSYNC;
    frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, p ? C_HOUR : 0x4208);
    frame.setTextColor(p ? TFT_BLACK : TFT_WHITE);
    frame.setTextDatum(middle_center);
    frame.setFont(&fonts::DejaVu12);
    frame.drawString("Wi-Fi", b->x + b->w / 2, b->y + 17);
    frame.setFont(&fonts::DejaVu18);
    frame.drawString("SYNC", b->x + b->w / 2, b->y + 37);
  }

  frame.drawRoundRect(10, 262, 300, 206, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("ALARM", 22, 276);
  drawSArrow(B_AH_UP, true);  drawSArrow(B_AH_DN, false);
  drawSArrow(B_AM_UP, true);  drawSArrow(B_AM_DN, false);
  drawFields(364, alarmH, alarmM, alarmOn ? C_HOUR : 0x8410);
  drawSButton(B_AON, alarmOn ? "ON" : "OFF", alarmOn ? C_OK : 0x4208, alarmOn ? TFT_BLACK : TFT_WHITE);
  // volume: label, - +, percentage
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_center);
  frame.drawString("VOLUME", 265, 356);
  drawSButton(B_VDN, "-", 0x4208, TFT_WHITE);
  drawSButton(B_VUP, "+", 0x4208, TFT_WHITE);
  char vb[8];
  snprintf(vb, sizeof(vb), "%d%%", alarmVol);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(alarmVol ? TFT_WHITE : C_BAD);
  frame.drawString(alarmVol ? vb : "MUTE", 265, 426);
  {                                      // current alarm sound (tap to change)
    String nm = alarmFile.length() ? alarmFile.substring(alarmFile.lastIndexOf('/') + 1) : String("beep");
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(pressedBtn == B_SOUND ? C_HOUR : (alarmFile.length() ? C_DATE : C_LABEL));
    frame.setTextDatum(middle_right);
    frame.drawString(fitText(nm, 170, false) + "  >", 298, 277);
  }

  if (millis() < savedMsgUntil) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(savedMsgColor);
    frame.setTextDatum(middle_center);
    frame.drawString(savedMsg, 160, 476 - 9);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

void updateClock(bool force);            // (defined below)

// ============================================================================
// BLUETOOTH (BLE, NORDIC UART SERVICE)
// ----------------------------------------------------------------------------
// Bluetooth Low Energy with the Nordic UART service, which terminal apps
// (Bluefruit Connect, nRF Connect) understand. A phone must pair with the
// 6-digit PIN first (encrypted, MITM-protected); until then writes are
// ignored. Paired phones send the same text commands as the Serial Monitor.
// ============================================================================
// Same scheme as the stepper clock: the phone must pair with the 6-digit
// PIN shown on Settings > Bluetooth (encrypted, MITM-protected) before it
// can send anything. Commands are the same as the Serial Monitor's.
const char* BT_NAME = "ClockNest";
#define NUS_SERVICE "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_RX      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_TX      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
uint32_t btPin = 0;
bool bleInited = false;
NimBLEServer* bleServer = nullptr;

// Bluetooth connection events (run in the BLE task): connect -> ask the phone
// to pair right away (iOS otherwise drops writes silently); disconnect ->
// advertise again; pairing done -> bleSecure says whether it succeeded.
class BtServerCB : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& info) override {
    bleConnected = true;
    bleSecure = false;
    Serial.println("BLE: phone connected, requesting pairing...");
    NimBLEDevice::startSecurity(info.getConnHandle());
  }
  void onDisconnect(NimBLEServer* s, NimBLEConnInfo& info, int reason) override {
    bleConnected = false;
    bleSecure = false;
    Serial.println("BLE: disconnected");
    if (btOn) NimBLEDevice::startAdvertising();
  }
  void onAuthenticationComplete(NimBLEConnInfo& info) override {
    bleSecure = info.isEncrypted() && info.isAuthenticated();
    Serial.println(bleSecure ? "BLE: paired & encrypted" : "BLE: pairing FAILED (wrong PIN?)");
  }
};

// A command arrived from the phone: copy it for loop() to run (never act
// in the BLE task itself), and ignore anything from an unpaired phone.
class BtRxCB : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& info) override {
    if (!bleSecure) return;
    NimBLEAttValue v = c->getValue();
    if (!bleHasCmd) {                    // loop() runs it
      bleInbox = String(v.c_str(), v.length());
      bleHasCmd = true;
    }
  }
};

// bleStart(): Starts Bluetooth. The first time it builds the BLE stack: secure pairing
//   (bonding + MITM + LE Secure Connections, 6-digit PIN shown on screen),
//   the Nordic UART service, and the advertising packets. Later calls just
//   restart advertising.
void bleStart() {
  if (!bleInited) {
    NimBLEDevice::init(BT_NAME);
    NimBLEDevice::setSecurityAuth(true, true, true);         // bonding, MITM, secure connections
    NimBLEDevice::setSecurityPasskey(btPin);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);  // the phone types the PIN
    bleServer = NimBLEDevice::createServer();
    bleServer->setCallbacks(new BtServerCB());
    NimBLEService* svc = bleServer->createService(NUS_SERVICE);
    txChar = svc->createCharacteristic(NUS_TX, NIMBLE_PROPERTY::NOTIFY);
    NimBLECharacteristic* rx = svc->createCharacteristic(NUS_RX,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR |
        NIMBLE_PROPERTY::WRITE_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN);
    rx->setCallbacks(new BtRxCB());
    svc->start();
    NimBLEAdvertisementData advData;
    advData.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    advData.addServiceUUID(NimBLEUUID(NUS_SERVICE));
    NimBLEAdvertisementData scanData;
    scanData.setName(BT_NAME);
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->setAdvertisementData(advData);
    adv->setScanResponseData(scanData);
    bleInited = true;
  }
  bool ok = NimBLEDevice::getAdvertising()->start();
  Serial.println(ok ? "BLE: on, advertising as ClockNest" : "BLE: advertising FAILED");
}

// bleDisconnectAll(): Drops every connected phone.
void bleDisconnectAll() {
  if (!bleServer) return;
  for (uint16_t h : bleServer->getPeerDevices()) bleServer->disconnect(h);
}

// bleStop(): Bluetooth 'off': stop advertising and drop phones (the stack stays loaded).
void bleStop() {
  if (!bleInited) return;
  NimBLEDevice::getAdvertising()->stop();
  bleDisconnectAll();
  Serial.println("BLE: off");
}

// setBt(): Bluetooth switch from the settings page / commands; saved in flash.
void setBt(bool on) {
  btOn = on;
  prefs.putBool("btOn", on);
  if (on) bleStart(); else bleStop();
}

// newBtPin(): Makes a new random 6-digit PIN, forgets all pairings (they were made with
//   the old PIN) and disconnects any phone.
void newBtPin() {
  btPin = 100000 + (esp_random() % 900000);
  prefs.putUInt("btPin", btPin);
  if (bleInited) {
    NimBLEDevice::setSecurityPasskey(btPin);
    NimBLEDevice::deleteAllBonds();      // old pairings no longer trusted
    bleDisconnectAll();
  }
}

// Print that copies command replies to the paired phone too. Collects a
// whole line first, then sends it in one go to USB and (if paired) to the
// phone in 20-byte pieces.
class TeeOut : public Print {
  String line;
public:
  size_t write(uint8_t c) override {
    line += (char)c;
    if (c == '\n' || line.length() >= 180) flushLine();
    return 1;
  }
  void flushLine() {
    Serial.write((const uint8_t*)line.c_str(), line.length());
    if (bleConnected && bleSecure && txChar) {
      for (unsigned int i = 0; i < line.length(); i += 20) {
        unsigned int e = i + 20 < line.length() ? i + 20 : line.length();
        String part = line.substring(i, e);
        txChar->setValue((uint8_t*)part.c_str(), part.length());
        txChar->notify();
        delay(5);
      }
    }
    line = "";
  }
};
TeeOut Out;

// ============================================================================
// SHARED PAGE WIDGETS
// ----------------------------------------------------------------------------
// Header and button helpers shared by the newer settings pages.
// ============================================================================
void drawPageBtn(int x, int y, int w, int h, int id, const char* label, uint16_t fill, uint16_t text, bool enabled) {
  if (pressedBtn == id && enabled) { fill = C_HOUR; text = TFT_BLACK; }
  frame.fillRoundRect(x, y, w, h, 10, fill);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(enabled ? text : 0x630C);
  frame.setTextDatum(middle_center);
  frame.drawString(label, x + w / 2, y + h / 2 + 1);
}

// drawPageHeader(): Standard page header: < BACK on the left, title, optional ON/OFF toggle.
void drawPageHeader(const char* title, bool hasToggle, bool on) {
  drawPageBtn(10, 8, 96, 44, B_BACK, "< BACK", C_DATE, TFT_BLACK, true);
  frame.setFont(hasToggle ? &fonts::DejaVu18 : &fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(title, hasToggle ? 160 : 196, 30);
  if (hasToggle) drawPageBtn(214, 8, 96, 44, B_TOGGLE, on ? "ON" : "OFF", on ? C_OK : C_PANEL, on ? TFT_BLACK : TFT_WHITE, true);
}

const Btn HDRBTNS[] = { { B_BACK, 10, 8, 96, 44 }, { B_TOGGLE, 214, 8, 96, 44 } };

// ============================================================================
// BLUETOOTH PAGE
// ----------------------------------------------------------------------------
// Settings > Bluetooth.
// ============================================================================
const Btn BTBTNS[] = {
  { B_BACK, 10, 8, 96, 44 }, { B_TOGGLE, 214, 8, 96, 44 },
  { B_BT_NEWPIN, 10, 338, 145, 52 }, { B_BT_FORGET, 165, 338, 145, 52 },
  { B_BT_DISC, 10, 400, 300, 52 },
};
const int NBTB = sizeof(BTBTNS) / sizeof(BTBTNS[0]);

// renderBtPage(): Settings > Bluetooth: name, status, the pairing PIN in large digits, help
//   text and the NEW PIN / FORGET / DISCONNECT buttons.
void renderBtPage() {
  frame.fillScreen(C_BG);
  drawPageHeader("Bluetooth", true, btOn);
  frame.drawRoundRect(10, 60, 300, 66, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_left);
  frame.drawString("NAME", 22, 76);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.drawString(BT_NAME, 70, 76);
  const char* st; uint16_t sc;
  if (!btOn) { st = "Off"; sc = C_DIM; }
  else if (bleSecure) { st = "Phone paired and connected"; sc = C_OK; }
  else if (bleConnected) { st = "Phone connected - pairing..."; sc = C_HOUR; }
  else { st = "On - waiting for a phone"; sc = C_COOL; }
  frame.setTextColor(sc);
  frame.drawString(fitText(st, 276, false), 22, 106);

  frame.drawRoundRect(10, 136, 300, 116, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_center);
  frame.drawString("PAIRING PIN - type it on your phone", 160, 154);
  char pin[8];
  snprintf(pin, sizeof(pin), "%06u", (unsigned)btPin);
  frame.setFont(&fonts::DejaVu56);
  frame.setTextColor(btOn ? C_HOUR : C_DIM);
  frame.drawString(pin, 160, 206);

  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.drawString("Pair from a BLE terminal app (nRF Connect,", 160, 278);
  frame.drawString("Bluefruit Connect). It takes the same", 160, 296);
  frame.drawString("commands as the Serial Monitor.", 160, 314);

  drawPageBtn(10, 338, 145, 52, B_BT_NEWPIN, "NEW PIN", C_PANEL, TFT_WHITE, true);
  drawPageBtn(165, 338, 145, 52, B_BT_FORGET, "FORGET", C_PANEL, TFT_WHITE, true);
  drawPageBtn(10, 400, 300, 52, B_BT_DISC, "DISCONNECT PHONE", C_PANEL, TFT_WHITE, bleConnected);
  if (millis() < savedMsgUntil) {
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(savedMsgColor);
    frame.drawString(savedMsg, 160, 470);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openBtPage(): Shows the Bluetooth page.
void openBtPage() { screen = SCREEN_BT; renderBtPage(); }

// ============================================================================
// WI-FI PAGE
// ----------------------------------------------------------------------------
// Settings > Wi-Fi: on/off, scan, choose a network.
// ============================================================================
struct WifiNet { String ssid; int rssi; bool secure; };   // one scanned network
std::vector<WifiNet> wifiNets;
bool wifiScanning = false;
int  wifiScroll = 0;
const char* wifiScanMsg = "";
const int WL_ROW_Y = 146, WL_ROW_H = 42, WL_ROWS = 6;
const Btn WIFIBTNS[] = {
  { B_BACK, 10, 8, 96, 44 }, { B_TOGGLE, 214, 8, 96, 44 },
  { B_W_UP, 10, 412, 56, 52 }, { B_W_DN, 72, 412, 56, 52 }, { B_W_SCAN, 134, 412, 176, 52 },
};
const int NWIFIB = sizeof(WIFIBTNS) / sizeof(WIFIBTNS[0]);

// wifiScanStart(): Starts an asynchronous network scan. If the radio is busy retrying a network
//   it can't reach, that is stopped first (scans fail otherwise).
void wifiScanStart() {
  if (!wifiOn || wifiScanning) return;
  if (WiFi.getMode() == WIFI_OFF) WiFi.mode(WIFI_STA);
  // a scan can't run while the radio keeps retrying a network it can't reach
  if (WiFi.status() != WL_CONNECTED) WiFi.disconnect(false, false);
  wifiNets.clear();
  wifiScroll = 0;
  wifiScanMsg = "";
  wifiScanning = (WiFi.scanNetworks(true) == WIFI_SCAN_RUNNING);
  if (!wifiScanning) wifiScanMsg = "scan failed - try again";
}

// wifiScanPoll(): Called from loop(): when the scan finishes, keeps one entry per network name
//   (strongest signal), drops hidden ones, sorts by signal, and resumes connecting
//   to the saved network if needed.
void wifiScanPoll() {                    // loop(): collect results when done
  if (!wifiScanning) return;
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_RUNNING) return;
  wifiScanning = false;
  if (n < 0) { wifiScanMsg = "scan failed - try again"; return; }
  for (int i = 0; i < n; i++) {
    String id = WiFi.SSID(i);
    if (id.length() == 0) continue;      // hidden network
    bool dup = false;
    for (auto& w : wifiNets) if (w.ssid == id) { dup = true; if (WiFi.RSSI(i) > w.rssi) w.rssi = WiFi.RSSI(i); }
    if (!dup) wifiNets.push_back({ id, (int)WiFi.RSSI(i), WiFi.encryptionType(i) != WIFI_AUTH_OPEN });
  }
  WiFi.scanDelete();
  std::sort(wifiNets.begin(), wifiNets.end(), [](const WifiNet& a, const WifiNet& b) { return a.rssi > b.rssi; });
  if (wifiNets.empty()) wifiScanMsg = "no networks found";
  // the scan stopped any retrying: resume the saved network if not connected
  if (WiFi.status() != WL_CONNECTED && wifiSsid.length()) wifiStart();
}

// drawSignal(): Four signal bars from an RSSI value (dBm).
void drawSignal(int x, int y, int rssi, uint16_t on) {   // 4 bars, x,y = bottom-left
  int bars = rssi > -55 ? 4 : rssi > -67 ? 3 : rssi > -78 ? 2 : 1;
  for (int i = 0; i < 4; i++) frame.fillRect(x + i * 5, y - 4 - i * 4, 3, 4 + i * 4, i < bars ? on : 0x2945);
}

// drawLock(): Small padlock for secured networks.
void drawLock(int x, int y, uint16_t c) {               // 9 x 12, top-left
  frame.drawCircle(x + 4, y + 4, 3, c);
  frame.fillRect(x, y + 5, 9, 7, c);
}

// setWifi(): Wi-Fi switch from the settings page / commands; saved in flash. Off really
//   turns the radio off (Nest, weather and time sync stop).
void setWifi(bool on) {
  wifiOn = on;
  prefs.putBool("wifiOn", on);
  if (on) { WiFi.mode(WIFI_STA); wifiStart(); wifiScanStart(); }
  else { WiFi.scanDelete(); wifiScanning = false; WiFi.disconnect(true); WiFi.mode(WIFI_OFF); wifiNets.clear(); }
}

// renderWifiPage(): Settings > Wi-Fi: status box, the scanned network list (signal, lock,
//   current network in green), scroll buttons and SCAN.
void renderWifiPage() {
  frame.fillScreen(C_BG);
  drawPageHeader("Wi-Fi", true, wifiOn);
  frame.drawRoundRect(10, 60, 300, 66, 12, C_WINRIM);
  bool conn = WiFi.status() == WL_CONNECTED;
  frame.setFont(&fonts::DejaVu18);
  frame.setTextDatum(middle_left);
  String l1, l2;
  uint16_t c1 = TFT_WHITE;
  if (!wifiOn) { l1 = "Off"; c1 = C_DIM; l2 = "Nest, weather and time sync need Wi-Fi"; }
  else if (conn) { l1 = "Connected: " + WiFi.SSID(); c1 = C_OK; l2 = WiFi.localIP().toString() + "   " + String(WiFi.RSSI()) + " dBm"; }
  else if (wifiSsid.length() && millis() - wifiJoinStart < 20000) { l1 = "Connecting to " + wifiSsid + "..."; c1 = C_HOUR; }
  else if (wifiSsid.length()) { l1 = "Not connected"; c1 = C_BAD; l2 = "can't join " + wifiSsid + " - check the password"; }
  else { l1 = "Not set up"; c1 = C_HOUR; l2 = "pick your network below"; }
  frame.setTextColor(c1);
  frame.drawString(fitText(l1, 276, false), 22, 82);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.drawString(fitText(l2, 276, false), 22, 108);

  frame.setTextColor(C_LABEL);
  frame.drawString("NETWORKS", 14, 136);
  frame.setTextDatum(middle_right);
  frame.setTextColor(wifiScanning ? C_HOUR : C_DIM);
  if (wifiScanning) frame.drawString("scanning...", 306, 136);
  else if (wifiScanMsg[0]) frame.drawString(wifiScanMsg, 306, 136);

  int n = wifiNets.size();
  for (int i = 0; i < WL_ROWS && wifiOn; i++) {
    int idx = wifiScroll + i;
    if (idx >= n) break;
    const WifiNet& w = wifiNets[idx];
    int y = WL_ROW_Y + i * WL_ROW_H, cy = y + WL_ROW_H / 2;
    bool cur = conn && w.ssid == WiFi.SSID();
    if (cur) frame.fillRoundRect(10, y + 2, 300, WL_ROW_H - 4, 6, 0x0200);
    frame.drawFastHLine(14, y + WL_ROW_H - 1, 292, 0x2104);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(cur ? C_OK : TFT_WHITE);
    frame.setTextDatum(middle_left);
    frame.drawString(fitText(w.ssid, 230, false), 16, cy);
    if (w.secure) drawLock(266, cy - 7, C_DIM);
    drawSignal(284, cy + 8, w.rssi, cur ? C_OK : C_LABEL);
  }
  bool canUp = wifiScroll > 0, canDn = wifiScroll + WL_ROWS < n;
  const Btn* bu = findBtn(WIFIBTNS, NWIFIB, B_W_UP);
  const Btn* bd = findBtn(WIFIBTNS, NWIFIB, B_W_DN);
  frame.fillRoundRect(bu->x, bu->y, bu->w, bu->h, 10, pressedBtn == B_W_UP && canUp ? C_HOUR : C_PANEL);
  drawArrowIn(bu, true, canUp ? C_LABEL : 0x4208);
  frame.fillRoundRect(bd->x, bd->y, bd->w, bd->h, 10, pressedBtn == B_W_DN && canDn ? C_HOUR : C_PANEL);
  drawArrowIn(bd, false, canDn ? C_LABEL : 0x4208);
  drawPageBtn(134, 412, 176, 52, B_W_SCAN, wifiScanning ? "SCANNING" : "SCAN", C_PANEL, TFT_WHITE, wifiOn && !wifiScanning);
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openWifiPage(): Shows the Wi-Fi page and starts a scan if there is no list yet.
void openWifiPage() {
  screen = SCREEN_WIFI;
  if (wifiOn && wifiNets.empty()) wifiScanStart();
  renderWifiPage();
}

// ============================================================================
// ON-SCREEN KEYBOARD (WI-FI PASSWORD)
// ----------------------------------------------------------------------------
// A small QWERTY keyboard for typing the Wi-Fi password: abc / ABC / two
// symbol pages, digits always on top, SHOW/HIDE for the password.
// ============================================================================
struct KbKey { int16_t x, y, w, h; int16_t code; };      // a keyboard key: rectangle + character
                                                          // (or a KC_ code for special keys)
enum { KC_SHIFT = 1, KC_DEL = 2, KC_SYM = 3, KC_OK = 4 };
KbKey keys[52];
int nKeys = 0, pressedKey = -1;
int kbMode = 0;                          // 0 abc, 1 ABC, 2 symbols, 3 more symbols
bool kbShow = false;
String kbSsid, kbPass;
const int KB_Y0 = 186, KB_KH = 50, KB_GAP = 6;

// addKeyRow(): Adds a row of character keys to the keyboard layout. Positions are in 'key
//   units' (31 px) so rows can be offset by half a key like a real keyboard.
void addKeyRow(const char* chars, float startUnit, int row) {
  for (int i = 0; chars[i]; i++) {
    float u = startUnit + i;
    keys[nKeys++] = { (int16_t)(5 + u * 31 + 1), (int16_t)(KB_Y0 + row * (KB_KH + KB_GAP)), 28, KB_KH, (int16_t)chars[i] };
  }
}

// addKey(): Adds one special key (SHIFT, DEL, #+=, space, JOIN) of a given width.
void addKey(float unit, float wUnits, int row, int code) {
  keys[nKeys++] = { (int16_t)(5 + unit * 31 + 1), (int16_t)(KB_Y0 + row * (KB_KH + KB_GAP)),
                    (int16_t)(wUnits * 31 - 3), KB_KH, (int16_t)code };
}

// buildKeys(): Builds the key list for the current keyboard mode: abc, ABC, symbols page 1
//   or 2. The digit row is always on top.
void buildKeys() {
  nKeys = 0;
  addKeyRow("1234567890", 0, 0);
  if (kbMode == 0) { addKeyRow("qwertyuiop", 0, 1); addKeyRow("asdfghjkl", 0.5f, 2); addKeyRow("zxcvbnm", 1.5f, 3); }
  else if (kbMode == 1) { addKeyRow("QWERTYUIOP", 0, 1); addKeyRow("ASDFGHJKL", 0.5f, 2); addKeyRow("ZXCVBNM", 1.5f, 3); }
  else if (kbMode == 2) { addKeyRow("!@#$%^&*()", 0, 1); addKeyRow("-_=+[]{}\\", 0.5f, 2); addKeyRow(";:'\",.?", 1.5f, 3); }
  else { addKeyRow("/<>`|~", 2, 1); }
  addKey(0, 1.5f, 3, KC_SHIFT);
  addKey(8.5f, 1.5f, 3, KC_DEL);
  addKey(0, 2, 4, KC_SYM);
  addKey(2, 5, 4, ' ');
  addKey(7, 3, 4, KC_OK);
}

// renderKbd(): The password keyboard page: network name, password field (masked unless
//   SHOW), and all keys; the key under your finger is amber.
void renderKbd() {
  frame.fillScreen(C_BG);
  drawPageBtn(10, 8, 96, 44, B_BACK, "< BACK", C_DATE, TFT_BLACK, true);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString("Password", 196, 30);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_left);
  frame.drawString(fitText("for " + kbSsid, 296, false), 14, 70);
  frame.drawRoundRect(10, 84, 240, 50, 8, C_HOUR);
  String shown = kbShow ? kbPass : String();
  if (!kbShow) for (unsigned i = 0; i < kbPass.length(); i++) shown += '*';
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.drawString(fitText(shown + "_", 224, true), 18, 110);
  drawPageBtn(258, 84, 52, 50, B_KB_SHOW, kbShow ? "HIDE" : "SHOW", C_PANEL, TFT_WHITE, true);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_center);
  frame.drawString("8-63 characters, case sensitive", 160, 160);

  for (int i = 0; i < nKeys; i++) {
    const KbKey& k = keys[i];
    bool special = k.code < 32;
    uint16_t fill = (i == pressedKey) ? C_HOUR : (k.code == KC_OK ? C_OK : (special ? 0x2945 : C_PANEL));
    if (k.code == KC_SHIFT && (kbMode == 1 || kbMode == 3)) fill = (i == pressedKey) ? C_HOUR : C_DATE;
    frame.fillRoundRect(k.x, k.y, k.w, k.h, 6, fill);
    uint16_t tc = (i == pressedKey || k.code == KC_OK || (k.code == KC_SHIFT && (kbMode == 1 || kbMode == 3))) ? TFT_BLACK : TFT_WHITE;
    frame.setTextColor(tc);
    frame.setTextDatum(middle_center);
    int cx = k.x + k.w / 2, cy = k.y + k.h / 2 + 1;
    if (special || k.code == ' ') {
      frame.setFont(&fonts::DejaVu12);
      const char* lab = k.code == KC_SHIFT ? (kbMode >= 2 ? (kbMode == 2 ? "1/2" : "2/2") : "SHIFT")
                      : k.code == KC_DEL ? "DEL" : k.code == KC_SYM ? (kbMode >= 2 ? "abc" : "#+=")
                      : k.code == KC_OK ? "JOIN" : "space";
      frame.drawString(lab, cx, cy);
    } else {
      char t[2] = { (char)k.code, 0 };
      frame.setFont(&fonts::DejaVu18);
      frame.drawString(t, cx, cy);
    }
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openKbd(): Opens the keyboard for a network (pre-fills the saved password if it's the
//   network we already know).
void openKbd(const String& ssid) {
  kbSsid = ssid;
  kbPass = (ssid == wifiSsid) ? wifiPass : String();
  kbMode = 0;
  kbShow = false;
  pressedKey = -1;
  buildKeys();
  screen = SCREEN_KBD;
  renderKbd();
}

// joinWifi(): Saves the chosen network + password, makes sure Wi-Fi is on, and connects.
void joinWifi(const String& ssid, const String& pass) {
  wifiSsid = ssid; wifiPass = pass;
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  if (!wifiOn) { wifiOn = true; prefs.putBool("wifiOn", true); }
  WiFi.disconnect(false, false);
  wifiStart();
  screen = SCREEN_WIFI;
  renderWifiPage();
}

// pressKey(): Handles one key press: case/symbol switching, delete, typing, and JOIN
//   (only with 8..63 characters, the WPA2 limits).
void pressKey(int i) {
  int c = keys[i].code;
  if (c == KC_SHIFT) { kbMode = (kbMode == 0) ? 1 : (kbMode == 1) ? 0 : (kbMode == 2) ? 3 : 2; buildKeys(); }
  else if (c == KC_SYM) { kbMode = (kbMode >= 2) ? 0 : 2; buildKeys(); }
  else if (c == KC_DEL) { if (kbPass.length()) kbPass.remove(kbPass.length() - 1); }
  else if (c == KC_OK) { if (kbPass.length() >= 8 && kbPass.length() <= 63) { joinWifi(kbSsid, kbPass); return; } }
  else if (kbPass.length() < 63) kbPass += (char)c;
  renderKbd();
}

// ============================================================================
// SD CARD
// ----------------------------------------------------------------------------
// microSD in 1-bit SDMMC mode on its own pins (CLK 11, CMD 10, D0 9), FAT
// file system via ESP-IDF at /sdcard. The card is only mounted while
// something uses it (SD page, alarm music), so it can be removed otherwise.
// ============================================================================
// The card slot uses the SDMMC peripheral in 1-bit mode on its own pins
// (CLK 11, CMD 10, D0 9 - from Waveshare's SD example); the expander keeps
// the card's D3 line high. The card is mounted only while the SD page (or an
// "sd" serial command) is using it, so it can be pulled out any other time.
const char* SD_BASE = "/sdcard";
void openSdPage();
void openClockPage();
extern bool sdPick, sdPickChime;
void openSoundPage();
bool isAudioName(const String& n);
const int SD_CLK = 11, SD_CMD = 10, SD_D0 = 9;
sdmmc_card_t* sdCard = nullptr;
bool sdMounted = false;
int  sdState = 0;                        // 0 not checked, 1 ok, 2 no card, 3 not FAT
struct SdEntry { String name; bool dir; uint64_t size; };  // one row of the file list
std::vector<SdEntry> sdEntries;
String sdPath = "/";
int  sdScroll = 0, sdSel = -1;
bool sdConfirm = false;
uint64_t sdTotal = 0, sdFree = 0;

const int SD_ROW_Y = 164, SD_ROW_H = 40, SD_ROWS = 6;
const Btn SDBTNS[] = {
  { B_BACK, 10, 8, 96, 44 }, { B_SD_USB, 214, 8, 96, 44 },
  { B_SD_UP, 10, 416, 56, 52 },   { B_SD_DN, 72, 416, 56, 52 },
  { B_SD_REFRESH, 134, 416, 86, 52 }, { B_SD_FORMAT, 226, 416, 84, 52 },
};
const Btn SDCBTNS[] = {                  // format confirmation dialog
  { B_SD_CANCEL, 40, 282, 110, 52 }, { B_SD_YES, 170, 282, 110, 52 },
};
const int NSDB = sizeof(SDBTNS) / sizeof(SDBTNS[0]);
const int NSDC = sizeof(SDCBTNS) / sizeof(SDCBTNS[0]);

bool usbDriveOn = false;                 // USB drive mode: the computer owns the card

// sdMount(): Mounts the card's FAT file system at /sdcard (SDMMC, 1-bit, own pins).
//   sdState: 1 ok, 2 no card, 3 card answers but has no FAT (e.g. exFAT).
//   With formatIfBad it formats an unreadable card while mounting.
//   Refused while the card belongs to the computer (USB drive mode).
esp_err_t sdMount(bool formatIfBad) {
  if (sdMounted) return ESP_OK;
  if (usbDriveOn) return ESP_ERR_INVALID_STATE;
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
  slot.width = 1;
  slot.clk = (gpio_num_t)SD_CLK;
  slot.cmd = (gpio_num_t)SD_CMD;
  slot.d0  = (gpio_num_t)SD_D0;
  slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
  esp_vfs_fat_sdmmc_mount_config_t mc = {};
  mc.format_if_mount_failed = formatIfBad;
  mc.max_files = 4;
  mc.allocation_unit_size = 32 * 1024;
  esp_err_t err = esp_vfs_fat_sdmmc_mount(SD_BASE, &host, &slot, &mc, &sdCard);
  if (err == ESP_OK) {
    sdMounted = true;
    sdState = 1;
    esp_vfs_fat_info(SD_BASE, &sdTotal, &sdFree);
  } else {
    sdCard = nullptr;
    sdState = (err == ESP_FAIL) ? 3 : 2;  // ESP_FAIL = card answered, no FAT on it
    Serial.printf("SD: mount failed (%s)\n", esp_err_to_name(err));
  }
  return err;
}

// sdUnmount(): Unmounts the card (safe to pull it out afterwards).
void sdUnmount() {
  if (!sdMounted) return;
  esp_vfs_fat_sdcard_unmount(SD_BASE, sdCard);
  sdCard = nullptr;
  sdMounted = false;
}

// sdFull(): Card path -> file system path ('/DCIM' -> '/sdcard/DCIM').
String sdFull(const String& path) {      // "/DCIM" -> "/sdcard/DCIM"
  return path == "/" ? String(SD_BASE) : String(SD_BASE) + path;
}

// sdLoadDir(): Reads the current folder into sdEntries: folders first, then files, A-Z,
//   with sizes. Hidden files (names starting with '.', e.g. the macOS '._' files)
//   are skipped. Max 400 entries.
void sdLoadDir() {
  sdEntries.clear();
  sdScroll = 0;
  sdSel = -1;
  if (!sdMounted) return;
  String full = sdFull(sdPath);
  DIR* d = opendir(full.c_str());
  if (!d) return;
  struct dirent* e;
  while ((e = readdir(d)) != nullptr && sdEntries.size() < 400) {
    if (e->d_name[0] == '.') continue;   // hidden: ._files, .Spotlight-V100, .Trashes...
    SdEntry en;
    en.name = e->d_name;
    en.dir = (e->d_type == DT_DIR);
    en.size = 0;
    if (!en.dir) {
      struct stat st;
      if (stat((full + "/" + en.name).c_str(), &st) == 0) en.size = st.st_size;
    }
    sdEntries.push_back(en);
  }
  closedir(d);
  std::sort(sdEntries.begin(), sdEntries.end(), [](const SdEntry& a, const SdEntry& b) {
    if (a.dir != b.dir) return a.dir;    // folders first
    return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
  });
  esp_vfs_fat_info(SD_BASE, &sdTotal, &sdFree);
}

// fmtBytes(): Human-readable size: B / KB / MB / GB.
String fmtBytes(uint64_t b) {
  char t[16];
  if (b < 1024) snprintf(t, sizeof(t), "%u B", (unsigned)b);
  else if (b < 1024ULL * 1024) snprintf(t, sizeof(t), "%.1f KB", b / 1024.0);
  else if (b < 1024ULL * 1024 * 1024) snprintf(t, sizeof(t), "%.1f MB", b / 1048576.0);
  else snprintf(t, sizeof(t), "%.1f GB", b / 1073741824.0);
  return String(t);
}

// sdCardType(): SDSC / SDHC/XC / MMC from the card's registers.
const char* sdCardType() {
  if (!sdCard) return "";
  if (sdCard->is_mmc) return "MMC";
  return (sdCard->ocr & (1UL << 30)) ? "SDHC/XC" : "SDSC";
}

// cut text to fit a width, with "..." at the end (or the start for paths)
String fitText(const String& s, int maxW, bool keepEnd) {
  if (frame.textWidth(s) <= maxW) return s;
  String t = s;
  while (t.length() > 1 && frame.textWidth(keepEnd ? "..." + t : t + "...") > maxW)
    t.remove(keepEnd ? 0 : t.length() - 1, 1);
  return keepEnd ? "..." + t : t + "...";
}

// drawSdIcon(): SD card silhouette with a cut corner and contacts (settings tile).
void drawSdIcon(int cx, int cy, uint16_t c) {
  int x = cx - 14, y = cy - 19;          // card 28 x 38 with a cut corner
  frame.fillRect(x + 9, y, 19, 38, c);
  frame.fillRect(x, y + 9, 28, 29, c);
  frame.fillTriangle(x, y + 9, x + 9, y, x + 9, y + 9, c);
  uint16_t bg = (c == TFT_BLACK) ? C_HOUR : C_PANEL;
  for (int i = 0; i < 4; i++) frame.fillRect(x + 10 + i * 4, y + 4, 2, 8, bg);
}

// drawFolderIcon(): Amber folder icon for the file list.
void drawFolderIcon(int x, int y, uint16_t c) {   // 22 x 16
  frame.fillRoundRect(x, y + 3, 22, 13, 2, c);
  frame.fillRoundRect(x, y, 9, 5, 1, c);
}

// drawFileIcon(): Page-with-lines icon for the file list.
void drawFileIcon(int x, int y, uint16_t c) {     // 16 x 20
  frame.drawRect(x + 2, y, 14, 20, c);
  frame.drawFastHLine(x + 5, y + 6, 8, c);
  frame.drawFastHLine(x + 5, y + 10, 8, c);
  frame.drawFastHLine(x + 5, y + 14, 8, c);
}

// sdHasUp(): true when the list needs a '..' row (we're inside a folder).
bool sdHasUp() { return sdMounted && sdPath != "/"; }

// sdRowCount(): Rows in the list including the '..' row.
int  sdRowCount() { return (int)sdEntries.size() + (sdHasUp() ? 1 : 0); }

// drawSdButton(): Button from any of the SD-page button lists; a null label draws an arrow.
void drawSdButton(const Btn* list, int n, int id, const char* label, uint16_t fill, uint16_t text, bool enabled) {
  const Btn* b = findBtn(list, n, id);
  if (pressedBtn == id && enabled) fill = C_HOUR, text = TFT_BLACK;
  frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, fill);
  if (!label) {                          // arrow buttons
    drawArrowIn(b, id == B_SD_UP, enabled ? text : 0x4208);
    return;
  }
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(enabled ? text : 0x630C);
  frame.setTextDatum(middle_center);
  frame.drawString(label, b->x + b->w / 2, b->y + b->h / 2 + 1);
}

// renderSd(): Settings > SD Card (also the alarm-sound picker): card info box, current
//   path, six-row file list, scroll / REFRESH / FORMAT (or BEEP in picker
//   mode), and the format confirmation dialog.
void renderSd() {
  frame.fillScreen(C_BG);
  drawSdButton(SDBTNS, NSDB, B_BACK, "< BACK", C_DATE, TFT_BLACK, true);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(sdPick ? (sdPickChime ? "Chime sound" : "Alarm sound") : "SD Card", 160, 30);
  if (!sdPick) drawSdButton(SDBTNS, NSDB, B_SD_USB, "USB", 0x3A6B, TFT_WHITE, sdState != 2);

  // card info
  frame.drawRoundRect(10, 60, 300, 78, 12, C_WINRIM);
  if (sdState == 1) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(TFT_WHITE);
    frame.setTextDatum(middle_left);
    char name[16];
    snprintf(name, sizeof(name), "%.8s", sdCard ? sdCard->cid.name : "");
    frame.drawString(String(sdCardType()) + "  " + name, 22, 80);
    frame.setTextDatum(middle_right);
    frame.drawString(fmtBytes(sdTotal), 298, 80);
    frame.fillRoundRect(22, 98, 276, 12, 4, C_PANEL);
    uint64_t used = sdTotal > sdFree ? sdTotal - sdFree : 0;
    int w = sdTotal ? (int)(276.0 * used / sdTotal) : 0;
    if (w > 0) frame.fillRoundRect(22, 98, max(w, 6), 12, 4, C_DATE);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(middle_left);
    frame.drawString(fmtBytes(used) + " used", 22, 124);
    frame.setTextDatum(middle_right);
    frame.drawString(fmtBytes(sdFree) + " free  -  FAT", 298, 124);
  } else {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_center);
    frame.setTextColor(sdState == 3 ? C_HOUR : C_LABEL);
    frame.drawString(sdState == 3 ? "Card not readable" : "No card found", 160, 86);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_DIM);
    frame.drawString(sdState == 3 ? "not FAT32 - FORMAT sets it up" : "insert a card, then REFRESH", 160, 114);
  }

  // path line (or a short message)
  frame.setFont(&fonts::DejaVu12);
  frame.setTextDatum(middle_left);
  if (millis() < savedMsgUntil) {
    frame.setTextColor(savedMsgColor);
    frame.drawString(savedMsg, 14, 151);
  } else if (sdMounted) {
    frame.setTextColor(C_LABEL);
    frame.drawString(fitText(sdPath, 210, true), 14, 151);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(middle_right);
    char cnt[16];
    snprintf(cnt, sizeof(cnt), "%d items", (int)sdEntries.size());
    frame.drawString(cnt, 306, 151);
  }

  // file list
  int n = sdRowCount();
  bool up = sdHasUp();
  for (int i = 0; i < SD_ROWS; i++) {
    int idx = sdScroll + i;
    if (idx >= n) break;
    int y = SD_ROW_Y + i * SD_ROW_H;
    if (idx == sdSel) frame.fillRoundRect(10, y + 2, 300, SD_ROW_H - 4, 6, C_PANEL);
    frame.drawFastHLine(14, y + SD_ROW_H - 1, 292, 0x2104);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextDatum(middle_left);
    int cy = y + SD_ROW_H / 2;
    if (up && idx == 0) {
      drawFolderIcon(16, cy - 8, C_DIM);
      frame.setTextColor(C_LABEL);
      frame.drawString(".. (up)", 48, cy);
      continue;
    }
    const SdEntry& e = sdEntries[idx - (up ? 1 : 0)];
    bool playable = !e.dir && isAudioName(e.name);
    bool chosen = playable && (sdPickChime ? chimeFile : alarmFile) == (sdPath == "/" ? String("") : sdPath) + "/" + e.name;
    if (e.dir) drawFolderIcon(16, cy - 8, C_HOUR);
    else       drawFileIcon(19, cy - 10, sdPick && playable ? C_DATE : C_DIM);
    String size = e.dir ? String(">") : fmtBytes(e.size);
    frame.setFont(&fonts::DejaVu12);
    int sw = frame.textWidth(size);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(middle_right);
    frame.drawString(size, 304, cy);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(e.dir ? TFT_WHITE : sdPick ? (chosen ? C_OK : playable ? TFT_WHITE : 0x4208) : C_LABEL);
    frame.setTextDatum(middle_left);
    frame.drawString(fitText(e.name, 304 - sw - 10 - 48, false), 48, cy);
  }
  if (sdMounted && n == 0) {
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(C_DIM);
    frame.setTextDatum(middle_center);
    frame.drawString("(empty)", 160, SD_ROW_Y + 60);
  }

  // buttons
  bool canUp = sdScroll > 0, canDn = sdScroll + SD_ROWS < n;
  drawSdButton(SDBTNS, NSDB, B_SD_UP, nullptr, C_PANEL, C_LABEL, canUp);
  drawSdButton(SDBTNS, NSDB, B_SD_DN, nullptr, C_PANEL, C_LABEL, canDn);
  drawSdButton(SDBTNS, NSDB, B_SD_REFRESH, "REFRESH", C_PANEL, TFT_WHITE, true);
  if (sdPick) {                          // back to the built-in sound: BEEP (alarm) / BELL (chime)
    bool own = (sdPickChime ? chimeFile : alarmFile).length() > 0;
    drawSdButton(SDBTNS, NSDB, B_SD_FORMAT, sdPickChime ? "BELL" : "BEEP", own ? C_PANEL : C_OK,
                 own ? TFT_WHITE : TFT_BLACK, true);
  }
  else drawSdButton(SDBTNS, NSDB, B_SD_FORMAT, "FORMAT", 0x7800, TFT_WHITE, sdState == 1 || sdState == 3);

  if (sdConfirm) {
    frame.fillRoundRect(20, 150, 280, 204, 16, 0x18C3);
    frame.drawRoundRect(20, 150, 280, 204, 16, C_BAD);
    frame.setTextDatum(middle_center);
    frame.setFont(&fonts::DejaVu24);
    frame.setTextColor(TFT_WHITE);
    frame.drawString("Format card?", 160, 186);
    frame.setFont(&fonts::DejaVu18);
    frame.setTextColor(C_LABEL);
    frame.drawString("Everything on it", 160, 226);
    frame.drawString("will be erased.", 160, 250);
    drawSdButton(SDCBTNS, NSDC, B_SD_CANCEL, "CANCEL", C_PANEL, TFT_WHITE, true);
    drawSdButton(SDCBTNS, NSDC, B_SD_YES, "FORMAT", C_BAD, TFT_WHITE, true);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// renderSdBusy(): Full-screen 'Reading card...' / 'Formatting...' message shown before slow
//   card operations (they block the main loop for a moment).
void renderSdBusy(const char* msg) {     // shown during slow card work
  frame.fillScreen(C_BG);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(msg, 160, 220);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.drawString("please wait - don't remove the card", 160, 256);
  frame.pushSprite(0, 0);
}

// sdNote(): Short colored message on the SD page's path line (2.5 s).
void sdNote(const char* m, uint16_t color) {
  savedMsg = m;
  savedMsgColor = color;
  savedMsgUntil = millis() + 2500;
}

// openSdPage(): Opens the SD page at the card's root (normal mode, not the picker).
void openSdPage() {
  sdPick = false;
  screen = SCREEN_SD;
  sdConfirm = false;
  sdPath = "/";
  renderSdBusy("Reading card...");
  sdMount(false);
  sdLoadDir();
  renderSd();
}

// sdRefresh(): Re-reads the card (after swapping it); stays in the same folder if it still exists.
void sdRefresh() {
  renderSdBusy("Reading card...");
  sdUnmount();
  sdMount(false);
  if (sdMounted) {                       // stay in the folder if it still exists
    struct stat st;
    if (stat(sdFull(sdPath).c_str(), &st) != 0) sdPath = "/";
  } else sdPath = "/";
  sdLoadDir();
}

// sdFormat(): Formats the card FAT32. A readable card is formatted in place; an unreadable
//   one is formatted by mounting with formatIfBad.
void sdFormat() {
  renderSdBusy("Formatting...");
  esp_err_t err;
  if (sdMounted) err = esp_vfs_fat_sdcard_format(SD_BASE, sdCard);
  else err = sdMount(true);              // unreadable card: mount formats it
  if (err == ESP_OK) {
    sdUnmount();                         // remount to read the fresh card
    sdMount(false);
  }
  sdPath = "/";
  sdLoadDir();
  if (err == ESP_OK && sdMounted) sdNote("Formatted (FAT32)", C_OK);
  else { static char e[48]; snprintf(e, sizeof(e), "Format failed: %s", esp_err_to_name(err)); sdNote(e, C_BAD); }
}

// a tap in the list area
void sdTapRow(int ty) {
  int row = (ty - SD_ROW_Y) / SD_ROW_H;
  int idx = sdScroll + row;
  if (row < 0 || row >= SD_ROWS || idx >= sdRowCount()) return;
  bool up = sdHasUp();
  if (up && idx == 0) {                  // ".." - one folder up
    int cut = sdPath.lastIndexOf('/');
    sdPath = cut <= 0 ? String("/") : sdPath.substring(0, cut);
    sdLoadDir();
    return;
  }
  const SdEntry& e = sdEntries[idx - (up ? 1 : 0)];
  if (e.dir) {
    sdPath = (sdPath == "/" ? "" : sdPath) + "/" + e.name;
    sdLoadDir();
  } else if (sdPick) {
    if (!isAudioName(e.name)) return;
    String f = (sdPath == "/" ? String("") : sdPath) + "/" + e.name;
    sdUnmount();
    sdPick = false;
    if (sdPickChime) {
      chimeFile = f; prefs.putString("chSnd", chimeFile);
      openSoundPage();
      savedMsg = "Chime sound set";
    } else {
      alarmFile = f; prefs.putString("alSnd", alarmFile);
      openClockPage();
      savedMsg = "Alarm sound set";
    }
    savedMsgColor = C_OK; savedMsgUntil = millis() + 1500;
  } else {
    sdSel = (sdSel == idx) ? -1 : idx;   // highlight a file
  }
}

// ============================================================================
// WEATHER SETTINGS PAGE: ZIP KEYPAD
// ----------------------------------------------------------------------------
// Settings > Weather: type a US ZIP code; the network task looks it up.
// ============================================================================
String zipEntry;
const Btn WXBTNS[] = {
  { B_BACK, 10, 8, 96, 44 },
  { B_K1, 15, 222, 90, 56 }, { B_K2, 115, 222, 90, 56 }, { B_K3, 215, 222, 90, 56 },
  { B_K4, 15, 284, 90, 56 }, { B_K5, 115, 284, 90, 56 }, { B_K6, 215, 284, 90, 56 },
  { B_K7, 15, 346, 90, 56 }, { B_K8, 115, 346, 90, 56 }, { B_K9, 215, 346, 90, 56 },
  { B_KDEL, 15, 408, 90, 56 }, { B_K0, 115, 408, 90, 56 }, { B_KOK, 215, 408, 90, 56 },
};
const int NWXB = sizeof(WXBTNS) / sizeof(WXBTNS[0]);

// drawKey(): Button from the ZIP keypad list (digits use a bigger font).
void drawKey(int id, const char* label, uint16_t fill, uint16_t text, bool enabled) {
  const Btn* b = findBtn(WXBTNS, NWXB, id);
  if (pressedBtn == id && enabled) { fill = C_HOUR; text = TFT_BLACK; }
  frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, fill);
  frame.setFont(id >= B_K0 && id <= B_K9 ? &fonts::DejaVu24 : &fonts::DejaVu18);
  frame.setTextColor(enabled ? text : 0x630C);
  frame.setTextDatum(middle_center);
  frame.drawString(label, b->x + b->w / 2, b->y + b->h / 2 + 1);
}

// renderWxPage(): Settings > Weather: the place now in use, five ZIP digit boxes, lookup
//   status, and the 0-9 / DEL / OK keypad.
void renderWxPage() {
  frame.fillScreen(C_BG);
  drawKey(B_BACK, "< BACK", C_DATE, TFT_BLACK, true);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString("Weather", 196, 30);

  // current place
  frame.drawRoundRect(10, 60, 300, 58, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_DIM);
  frame.setTextDatum(middle_left);
  frame.drawString("FORECAST FOR", 22, 74);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(wxHaveLoc ? TFT_WHITE : C_DIM);
  frame.drawString(fitText(wxHaveLoc ? wxPlace : String("not set"), 276, false), 22, 98);

  // ZIP entry: five boxes
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("ZIP CODE", 22, 134);
  for (int i = 0; i < 5; i++) {
    int x = 40 + i * 50;
    bool cur = (i == (int)zipEntry.length());
    frame.drawRoundRect(x, 146, 42, 52, 8, cur ? C_HOUR : C_WINRIM);
    if (i < (int)zipEntry.length()) {
      char d[2] = { zipEntry[i], 0 };
      frame.setFont(&fonts::DejaVu40);
      frame.setTextColor(TFT_WHITE);
      frame.setTextDatum(middle_center);
      frame.drawString(d, x + 21, 173);
    }
  }
  // lookup status
  frame.setFont(&fonts::DejaVu12);
  frame.setTextDatum(middle_center);
  if (wxLookup == 1) { frame.setTextColor(C_HOUR); frame.drawString("looking up...", 160, 210); }
  else if (wxLookup == 2) { frame.setTextColor(C_OK); frame.drawString(wxLookupMsg, 160, 210); }
  else if (wxLookup == 3) { frame.setTextColor(C_BAD); frame.drawString(wxLookupMsg, 160, 210); }
  else if (WiFi.status() != WL_CONNECTED) { frame.setTextColor(C_BAD); frame.drawString("no Wi-Fi - can't look up", 160, 210); }

  // keypad
  for (int k = 1; k <= 9; k++) { char d[2] = { (char)('0' + k), 0 }; drawKey(B_K0 + k, d, C_PANEL, TFT_WHITE, zipEntry.length() < 5); }
  drawKey(B_K0, "0", C_PANEL, TFT_WHITE, zipEntry.length() < 5);
  drawKey(B_KDEL, "DEL", C_PANEL, TFT_WHITE, zipEntry.length() > 0);
  drawKey(B_KOK, "OK", C_OK, TFT_BLACK, zipEntry.length() == 5 && wxLookup != 1);

  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openWxPage(): Shows the Weather page with the saved ZIP pre-filled.
void openWxPage() {
  screen = SCREEN_WX;
  zipEntry = prefs.getString("wxZip", "");
  if (wxLookup != 1) wxLookup = 0;
  renderWxPage();
}

// ============================================================================
// ALARM MUSIC (SD CARD)
// ----------------------------------------------------------------------------
// MP3 (and AAC/FLAC/WAV/OGG) for the alarm, decoded by the ESP32-audioI2S
// library (Waveshare's copy, v3.3.0). The library runs its own task; our
// job is to feed it (musicService) and pass its output to our I2S.
// ============================================================================
// ESP32-audioI2S decodes the file; its output (48 kHz stereo) is copied to
// our own I2S (the one the beeps use) in audio_process_i2s(). The library's
// own I2S channel (I2S1) runs too, wired to no pin: it paces the decoder and
// keeps the library's buffer bookkeeping right.
class CardFS : public fs::FS {             // our FAT mount (/sdcard) as an Arduino FS
public:
  CardFS() : fs::FS(fs::FSImplPtr(new VFSImpl())) {}
  void setMount(const char* m) { _impl->mountpoint(m); }
};
CardFS cardFs;
Audio* player = nullptr;
bool musicMountedSd = false;             // we mounted the card just for the alarm

volatile uint32_t musicChunks = 0, musicBytes = 0;

// audio_process_i2s(): Called by the audio library (from its own task) with every decoded block of
//   48 kHz stereo samples. We copy them to our I2S output (the one the beep uses);
//   continueI2S = true lets the library finish its own bookkeeping on its
//   pin-less I2S1 channel, which also paces the decoder.
void audio_process_i2s(int16_t* outBuff, uint16_t validSamples, bool* continueI2S) {
  if ((alarmMusic || chimeMusic) && audioOk && validSamples) {
    musicBytes += i2s.write((const uint8_t*)outBuff, (size_t)validSamples * 4);
    musicChunks++;
  }
  *continueI2S = true;                   // let the library finish the chunk (pin-less I2S1)
}

// audio_info(): The audio library's status messages, printed to Serial ('audio: ...').
void audio_info(const char* info) { Serial.print("audio: "); Serial.println(info); }

// isAudioName(): true for file names the decoder can play (mp3 m4a aac flac wav ogg opus).
bool isAudioName(const String& n) {
  String l = n; l.toLowerCase();
  return l.endsWith(".mp3") || l.endsWith(".m4a") || l.endsWith(".aac") || l.endsWith(".flac") ||
         l.endsWith(".wav") || l.endsWith(".ogg") || l.endsWith(".opus");
}

// startAlarmMusic(): Starts playing alarmFile for the alarm: mounts the card if needed, checks the
//   file exists, creates the player on first use, starts it. Returns false (and
//   the caller beeps instead) if anything is missing.
bool startAlarmMusic() {
  if (alarmFile.length() == 0 || !audioOk || usbDriveOn) return false;
  if (!sdMounted) {
    if (sdMount(false) != ESP_OK) { Serial.println("Alarm sound: no card - beeping"); return false; }
    musicMountedSd = true;
  }
  struct stat st;
  if (stat(sdFull(alarmFile).c_str(), &st) != 0) {
    Serial.println("Alarm sound: " + alarmFile + " not found - beeping");
    if (musicMountedSd) { sdUnmount(); musicMountedSd = false; }
    return false;
  }
  if (!player) {
    player = new Audio(I2S_NUM_1);
    player->setPinout(255, 255, 255, -1);  // allocates buffers; pins unused (see above)
    player->setVolume(21);
  }
  alarmMusic = true;
  musicChunks = musicBytes = 0;
  if (!player->connecttoFS(cardFs, alarmFile.c_str())) {
    alarmMusic = false;
    Serial.println("Alarm sound: can't play " + alarmFile + " - beeping");
    if (musicMountedSd) { sdUnmount(); musicMountedSd = false; }
    return false;
  }
  Serial.println("Alarm sound: " + alarmFile);
  return true;
}

// stopAlarmMusic(): Stops the music and unmounts the card again if we mounted it just for this.
void stopAlarmMusic() {
  if (!alarmMusic) return;
  alarmMusic = false;
  if (player) player->stopSong();
  if (musicMountedSd && screen != SCREEN_SD) sdUnmount();
  musicMountedSd = false;
}

// startChimeMusic(): Plays chimeFile once for the hourly chime (same player as
//   the alarm music). false = no card / file / decoder: the caller rings the bell.
unsigned long chimeMusicStart = 0;
bool startChimeMusic() {
  if (chimeFile.length() == 0 || !audioOk || usbDriveOn || alarmMusic) return false;
  if (!sdMounted) {
    if (sdMount(false) != ESP_OK) { Serial.println("Chime sound: no card - bell"); return false; }
    musicMountedSd = true;
  }
  struct stat st;
  if (stat(sdFull(chimeFile).c_str(), &st) != 0) {
    Serial.println("Chime sound: " + chimeFile + " not found - bell");
    if (musicMountedSd) { sdUnmount(); musicMountedSd = false; }
    return false;
  }
  if (!player) {
    player = new Audio(I2S_NUM_1);
    player->setPinout(255, 255, 255, -1);
    player->setVolume(21);
  }
  chimeMusic = true;
  chimeMusicStart = millis();
  if (!player->connecttoFS(cardFs, chimeFile.c_str())) {
    chimeMusic = false;
    Serial.println("Chime sound: can't play " + chimeFile + " - bell");
    if (musicMountedSd) { sdUnmount(); musicMountedSd = false; }
    return false;
  }
  return true;
}

// stopChimeMusic(): Stops a chime file (finished, or the alarm takes over).
void stopChimeMusic() {
  if (!chimeMusic) return;
  chimeMusic = false;
  if (player) player->stopSong();
  if (musicMountedSd && screen != SCREEN_SD) sdUnmount();
  musicMountedSd = false;
}

// musicService(): Called every loop(): feeds the decoder from the file (the library needs
//   loop() calls for that) and restarts the file when it ends, so the alarm
//   keeps playing until it is stopped. A chime file plays once.
void musicService() {                    // loop(): feed the decoder, repeat the file
  if (chimeMusic && player) {
    player->loop();
    if (!player->isRunning() && millis() - chimeMusicStart > 300) stopChimeMusic();   // played to the end
    return;
  }
  if (!alarmMusic || !player) return;
  player->loop();                        // reads the file into the decoder's buffer
  if (alarmRinging && !player->isRunning())
    player->connecttoFS(cardFs, alarmFile.c_str());
}

// ringClockAlarm(): Rings the clock alarm (test = the short "alarm test"
//   command): the SD file if one is set (looping, up to 5 min), else the beep
//   for 1 min. Returns to the clock face first so the flashing bell shows.
void closeSettings();
void ringClockAlarm(bool test) {
  ringLabel = "ALARM";
  endChime();                            // free the player / I2S for the alarm
  // back to the clock face so the flashing bell is in view (not from USB drive mode)
  if (!test && screen != SCREEN_FACE && screen != SCREEN_USB) closeSettings();
  if (startAlarmMusic()) startAlarm(test ? 10000UL : 5UL * 60UL * 1000UL);
  else startAlarm(test ? 3000UL : 60000UL);            // beep: 1 minute
}

// SD page in "pick a sound" mode: for the alarm (Clock page) or the chime (Sound page)
bool sdPick = false, sdPickChime = false;
void openClockPage();

// openSoundPicker(): Opens the SD page in 'Alarm sound' / 'Chime sound' picker mode.
void openSoundPicker(bool forChime) {
  openSdPage();
  sdPick = true;
  sdPickChime = forChime;
  renderSd();
}

// ============================================================================
// USB DRIVE MODE
// ----------------------------------------------------------------------------
// The card shows up on the computer as a USB disk (USB Mass Storage). Needs
// Tools > USB Mode: USB-OTG (TinyUSB). While it's on, the computer owns the
// card's file system and the clock doesn't touch it.
// ============================================================================
// The card is unmounted here and handed to the computer as a USB disk
// (raw sectors over USB mass storage). The clock doesn't touch the card
// until DONE. Needs Tools > USB Mode: USB-OTG (TinyUSB).
volatile uint32_t usbReadSec = 0, usbWriteSec = 0;
volatile bool usbEjected = false;
unsigned long usbLastActivity = 0;
const char* usbMsg = "";

#if HAVE_USB_DRIVE
USBMSC msc;
sdmmc_card_t* rawCard = nullptr;
uint8_t* secBuf = nullptr;               // one-sector bounce buffer (DMA-capable)

// rawCardInit(): USB drive mode: starts the SD card at the sector level (no file system), so
//   the computer can own the FAT. Also allocates a one-sector DMA buffer.
bool rawCardInit() {
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
  slot.width = 1;
  slot.clk = (gpio_num_t)SD_CLK;
  slot.cmd = (gpio_num_t)SD_CMD;
  slot.d0  = (gpio_num_t)SD_D0;
  slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
  if (sdmmc_host_init() != ESP_OK) return false;
  if (sdmmc_host_init_slot(host.slot, &slot) != ESP_OK) { sdmmc_host_deinit(); return false; }
  rawCard = (sdmmc_card_t*)calloc(1, sizeof(sdmmc_card_t));
  if (!rawCard || sdmmc_card_init(&host, rawCard) != ESP_OK) {
    free(rawCard); rawCard = nullptr; sdmmc_host_deinit(); return false;
  }
  if (!secBuf) secBuf = (uint8_t*)heap_caps_malloc(512, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  return true;
}

// rawCardEnd(): Releases the card from USB drive mode.
void rawCardEnd() {
  if (!rawCard) return;
  sdmmc_host_deinit();
  free(rawCard);
  rawCard = nullptr;
}

// TinyUSB calls these from its own task
static int32_t mscRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  if (!rawCard) return -1;
  uint8_t* out = (uint8_t*)buffer;
  uint32_t done = 0;
  while (done < bufsize) {
    uint32_t off = (offset + done) % 512, sec = lba + (offset + done) / 512;
    uint32_t n = min((uint32_t)512 - off, bufsize - done);
    if (off == 0 && n == 512 && (bufsize - done) >= 512) {
      uint32_t whole = (bufsize - done) / 512;
      if (sdmmc_read_sectors(rawCard, out + done, sec, whole) != ESP_OK) return -1;
      done += whole * 512;
    } else {
      if (sdmmc_read_sectors(rawCard, secBuf, sec, 1) != ESP_OK) return -1;
      memcpy(out + done, secBuf + off, n);
      done += n;
    }
  }
  usbReadSec += bufsize / 512;
  usbLastActivity = millis();
  return bufsize;
}

// mscWrite(): Computer -> card: writes sectors. Partial sectors are read, patched and
//   written back. Runs in the USB task.
static int32_t mscWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  if (!rawCard) return -1;
  uint32_t done = 0;
  while (done < bufsize) {
    uint32_t off = (offset + done) % 512, sec = lba + (offset + done) / 512;
    uint32_t n = min((uint32_t)512 - off, bufsize - done);
    if (off == 0 && n == 512 && (bufsize - done) >= 512) {
      uint32_t whole = (bufsize - done) / 512;
      if (sdmmc_write_sectors(rawCard, buffer + done, sec, whole) != ESP_OK) return -1;
      done += whole * 512;
    } else {                             // partial sector: read, patch, write
      if (sdmmc_read_sectors(rawCard, secBuf, sec, 1) != ESP_OK) return -1;
      memcpy(secBuf + off, buffer + done, n);
      if (sdmmc_write_sectors(rawCard, secBuf, sec, 1) != ESP_OK) return -1;
      done += n;
    }
  }
  usbWriteSec += bufsize / 512;
  usbLastActivity = millis();
  return bufsize;
}

// mscStartStop(): The computer ejected the disk (we show 'Ejected' and allow DONE).
static bool mscStartStop(uint8_t power_condition, bool start, bool load_eject) {
  if (load_eject && !start) usbEjected = true;   // ejected on the computer
  return true;
}

// usbDriveSetup(): setup(): registers an empty USB disk so the USB descriptors include it
//   from power-up (TinyUSB can't add it later).
void usbDriveSetup() {                   // setup(): register the (empty) drive
  msc.vendorID("ClockNst");
  msc.productID("SD Card");
  msc.productRevision("1.0");
  msc.onRead(mscRead);
  msc.onWrite(mscWrite);
  msc.onStartStop(mscStartStop);
  msc.mediaPresent(false);
  msc.begin(0, 512);
}
#endif

// usbDriveStart(): Enters USB drive mode: unmount FAT, start the card raw, publish its size,
//   and report 'media present' so the Mac mounts it.
bool usbDriveStart() {
#if HAVE_USB_DRIVE
  sdUnmount();
  if (!rawCardInit()) { usbMsg = "Card not found"; return false; }
  usbReadSec = usbWriteSec = 0;
  usbEjected = false;
  usbLastActivity = 0;
  uint32_t sectors = rawCard->csd.capacity;
  msc.begin(sectors, 512);
  msc.mediaPresent(true);
  usbDriveOn = true;
  usbMsg = "";
  Serial.printf("USB drive: %lu sectors (%s)\n", (unsigned long)sectors, fmtBytes((uint64_t)sectors * 512).c_str());
  return true;
#else
  usbMsg = "needs USB-OTG mode";
  return false;
#endif
}

// usbDriveStop(): Leaves USB drive mode: 'media gone' to the computer, wait for any last
//   transfer, release the card.
void usbDriveStop() {
#if HAVE_USB_DRIVE
  if (!usbDriveOn) return;
  msc.mediaPresent(false);
  delay(300);                            // let any last transfer finish
  rawCardEnd();
  usbDriveOn = false;
  Serial.println("USB drive: off");
#endif
}

const Btn USBBTNS[] = { { B_USB_DONE, 10, 412, 300, 56 } };

// drawUsbGlyph(): USB trident symbol.
void drawUsbGlyph(int cx, int cy, uint16_t c) {          // trident, ~60 px tall
  frame.drawWideLine(cx, cy - 26, cx, cy + 24, 2.5f, c);
  frame.fillTriangle(cx - 7, cy - 22, cx + 7, cy - 22, cx, cy - 32, c);
  frame.drawWideLine(cx, cy + 8, cx - 16, cy - 4, 2.5f, c);
  frame.drawWideLine(cx - 16, cy - 4, cx - 16, cy - 12, 2.5f, c);
  frame.fillCircle(cx - 16, cy - 15, 4, c);
  frame.drawWideLine(cx, cy + 14, cx + 16, cy + 2, 2.5f, c);
  frame.drawWideLine(cx + 16, cy + 2, cx + 16, cy - 8, 2.5f, c);
  frame.fillRect(cx + 12, cy - 16, 9, 9, c);
  frame.fillCircle(cx, cy + 28, 6, c);
}

// renderUsbPage(): USB drive page: symbol (amber while data moves), instructions, read/written
//   totals, and DONE (greyed while a copy is in progress).
void renderUsbPage() {
  frame.fillScreen(C_BG);
  frame.setTextDatum(middle_center);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.drawString("USB Drive", 160, 30);
  bool busy = usbDriveOn && millis() - usbLastActivity < 400 && usbLastActivity;
  drawUsbGlyph(160, 120, !usbDriveOn ? C_DIM : (busy ? C_HOUR : C_DATE));
  frame.setFont(&fonts::DejaVu18);
  if (usbDriveOn && !usbEjected) {
    frame.setTextColor(C_OK);
    frame.drawString("The card is on your computer", 160, 196);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_LABEL);
    frame.drawString("It shows up as a disk (often named NO NAME).", 160, 228);
    frame.drawString("Copy files in Finder, then EJECT the disk", 160, 250);
    frame.drawString("on the Mac before you tap DONE.", 160, 270);
    char b[48];
    snprintf(b, sizeof(b), "read %s   written %s", fmtBytes((uint64_t)usbReadSec * 512).c_str(),
             fmtBytes((uint64_t)usbWriteSec * 512).c_str());
    frame.setTextColor(busy ? C_HOUR : C_DIM);
    frame.drawString(b, 160, 312);
    frame.drawString(busy ? "copying..." : "idle", 160, 334);
  } else if (usbDriveOn && usbEjected) {
    frame.setTextColor(C_OK);
    frame.drawString("Ejected on the computer", 160, 196);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_LABEL);
    frame.drawString("Safe to tap DONE.", 160, 228);
  } else {
    frame.setTextColor(C_BAD);
    frame.drawString(HAVE_USB_DRIVE ? usbMsg : "Not available in this build", 160, 196);
    frame.setFont(&fonts::DejaVu12);
    frame.setTextColor(C_LABEL);
    if (!HAVE_USB_DRIVE) {
      frame.drawString("In the Arduino IDE, Tools menu, set:", 160, 230);
      frame.drawString("USB Mode: USB-OTG (TinyUSB)", 160, 252);
      frame.drawString("Upload Mode: USB-OTG CDC (TinyUSB)", 160, 272);
      frame.drawString("then upload again.", 160, 292);
    } else frame.drawString("Check the card and try again.", 160, 230);
  }
  bool doneOk = !usbDriveOn || usbEjected || millis() - usbLastActivity > 1500 || !usbLastActivity;
  uint16_t fill = pressedBtn == B_USB_DONE ? C_HOUR : (doneOk ? C_DATE : C_PANEL);
  frame.fillRoundRect(10, 412, 300, 56, 10, fill);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(doneOk || pressedBtn == B_USB_DONE ? TFT_BLACK : 0x630C);
  frame.drawString(usbDriveOn ? "DONE" : "BACK", 160, 441);
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

void openSdPage();

// openUsbPage(): Starts USB drive mode and shows its page.
void openUsbPage() {
  screen = SCREEN_USB;
  usbDriveStart();
  renderUsbPage();
}

// pressUsb(): DONE: only when idle / ejected; then gives the card back and reopens the SD page.
void pressUsb(int id) {
  if (id != B_USB_DONE) return;
  bool doneOk = !usbDriveOn || usbEjected || millis() - usbLastActivity > 1500 || !usbLastActivity;
  if (!doneOk) return;                   // don't pull the card mid-copy
  usbDriveStop();
  pressedBtn = B_NONE;
  openSdPage();
}

void closeSettings();                    // (defined below)
void renderDispPage();
void openDispPage();
void openSoundPage();
void renderSoundPage();
void drawBrightGlyph(int cx, int cy, uint16_t c);

// ---- settings grid ----
void drawClockIcon(int cx, int cy, uint16_t c) {         // ~40 px
  frame.drawCircle(cx, cy, 20, c);
  frame.drawCircle(cx, cy, 19, c);
  frame.drawWideLine(cx, cy, cx, cy - 13, 1.5f, c);       // minute hand
  frame.drawWideLine(cx, cy, cx + 9, cy + 3, 1.5f, c);    // hour hand
  frame.fillCircle(cx, cy, 3, C_HOUR);
}

// drawRadioGlyph(): Old radio (Radio tile): body, speaker grille, dial, antenna.
void drawRadioGlyph(int cx, int cy, uint16_t c) {        // ~40 x 34
  frame.drawWideLine(cx - 10, cy - 9, cx + 12, cy - 18, 1.5f, c);   // antenna
  frame.drawRoundRect(cx - 20, cy - 9, 40, 26, 5, c);
  frame.drawRoundRect(cx - 19, cy - 8, 38, 24, 4, c);
  frame.drawCircle(cx - 8, cy + 4, 7, c);                 // speaker
  frame.drawCircle(cx - 8, cy + 4, 3, c);
  for (int i = 0; i < 3; i++) frame.drawFastHLine(cx + 4, cy - 2 + i * 5, 11, c);   // dial
}

// drawTile(): One settings tile (icon, title, subtitle); amber while pressed.
//   The Radio tile is a placeholder: drawn dim, not a button.
void drawTile(int col, int row, int id, const char* title, const char* sub) {
  int x = TILE_X[col], y = TILE_Y[row];
  bool pressed = (pressedBtn == id);
  bool soon = (id == B_TILE_RADIO);
  frame.fillRoundRect(x, y, TILE_W, TILE_H, 14, pressed ? C_HOUR : C_PANEL);
  uint16_t fg = pressed ? TFT_BLACK : soon ? 0x4208 : TFT_WHITE;
  int ix = x + TILE_W / 2, iy = y + 31;  // icon centre
  if (id == B_TILE_CLOCK) drawClockIcon(ix, iy, fg);
  if (id == B_TILE_SD)    drawSdIcon(ix, iy, fg);
  if (id == B_TILE_WX)    drawWxIcon(ix, iy, 52, 2, true);
  if (id == B_TILE_WIFI)  drawWifiGlyph(ix, iy - 4, 1.3f, fg);
  if (id == B_TILE_BT)    drawBtGlyph(ix, iy, 1.3f, fg);
  if (id == B_TILE_DISP)  drawBrightGlyph(ix, iy, fg);
  if (id == B_TILE_SOUND) drawNoteGlyph(ix - 2, iy - 2, 1.4f, pressed ? TFT_BLACK : C_HOUR, false);
  if (id == B_TILE_RADIO) drawRadioGlyph(ix, iy, fg);
  frame.setTextDatum(middle_center);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(soon ? 0x630C : fg);
  frame.drawString(title, x + TILE_W / 2, y + 64);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(pressed ? TFT_BLACK : soon ? 0x4208 : C_DIM);
  frame.drawString(sub, x + TILE_W / 2, y + 80);
}

// renderMenu(): The settings grid: 2 x 4 tiles, DONE, and a handle at the
//   bottom (swipe up to close). Respects holdPush for the slide animations.
void renderMenu() {
  frame.fillScreen(C_BG);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_left);
  frame.drawString("Settings", 14, 30);
  const Btn* d = findBtn(MBTNS, NMB, B_DONE);
  frame.fillRoundRect(d->x, d->y, d->w, d->h, 10, pressedBtn == B_DONE ? C_HOUR : C_DATE);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_BLACK);
  frame.setTextDatum(middle_center);
  frame.drawString("DONE", d->x + d->w / 2, d->y + d->h / 2 + 1);

  drawTile(0, 0, B_TILE_CLOCK, "Clock", "time - alarm");
  drawTile(1, 0, B_TILE_DISP, "Display", "light - sleep");
  drawTile(0, 1, B_TILE_WX, "Weather", "ZIP code");
  drawTile(1, 1, B_TILE_WIFI, "Wi-Fi", wifiOn ? (WiFi.status() == WL_CONNECTED ? "connected" : "on") : "off");
  drawTile(0, 2, B_TILE_BT, "Bluetooth", btOn ? (bleSecure ? "phone paired" : "on") : "off");
  drawTile(1, 2, B_TILE_SD, "SD Card", "files - format");
  drawTile(0, 3, B_TILE_SOUND, "Sound", chimeOn ? "chime - quiet" : "chime off");
  drawTile(1, 3, B_TILE_RADIO, "Radio", "coming soon");
  frame.fillRoundRect(W / 2 - 22, H - 9, 44, 4, 2, 0x4208);  // handle: swipe up to close
  if (alarmRinging) drawRinging();
  if (!holdPush) frame.pushSprite(0, 0);
}

// ---- the settings "shade": slides down over the clock, and back up ----
// Open: the face stays on the display; the grid is drawn off-screen and
// pushed in steps from above (the display clips what's off the top).
// slideSettingsOpen(): see above. About 12 steps, ease-out, ~0.2 s.
void slideSettingsOpen() {
  screen = SCREEN_MENU;
  lastTouchMs = millis();
  pressedBtn = B_NONE;
  holdPush = true;
  renderMenu();                          // grid into the frame, not shown yet
  holdPush = false;
  const int N = 12;
  for (int i = 1; i <= N; i++) {
    float t = (float)i / N, e = 1 - (1 - t) * (1 - t) * (1 - t);
    frame.pushSprite(0, -H + (int)(H * e));
  }
  frame.pushSprite(0, 0);
}

// slideSettingsClosed(): the grid slides up off the screen, revealing the
// face underneath. Needs both pictures at once, so the grid is copied into a
// temporary second frame (PSRAM) first. Falls back to a plain switch if that
// memory isn't available.
void closeSettings();
void slideSettingsClosed() {
  LGFX_Sprite shade(&lcd);
  shade.setPsram(true);
  shade.setColorDepth(16);
  bool ok = shade.createSprite(W, H);
  if (ok) memcpy(shade.getBuffer(), frame.getBuffer(), (size_t)W * H * 2);   // the grid as shown
  holdPush = true;
  closeSettings();                       // face into the frame, not shown yet
  holdPush = false;
  if (ok) {
    const int N = 12;
    for (int i = 1; i <= N; i++) {
      float t = (float)i / N, e = t * t * (3 - 2 * t);
      int y = -(int)(H * e);             // grid's top edge, moving up
      lcd.setClipRect(0, y + H, W, -y);  // face only in the uncovered strip
      frame.pushSprite(0, 0);
      lcd.clearClipRect();
      shade.pushSprite(0, y);
    }
    shade.deleteSprite();
  }
  frame.pushSprite(0, 0);
}

// renderCurrent(): Redraws whichever settings page is on screen.
void renderCurrent() {
  if (screen == SCREEN_MENU) renderMenu();
  else if (screen == SCREEN_SETTINGS) renderSettings();
  else if (screen == SCREEN_SD) renderSd();
  else if (screen == SCREEN_WX) renderWxPage();
  else if (screen == SCREEN_WIFI) renderWifiPage();
  else if (screen == SCREEN_KBD) renderKbd();
  else if (screen == SCREEN_BT) renderBtPage();
  else if (screen == SCREEN_DISP) renderDispPage();
  else if (screen == SCREEN_USB) renderUsbPage();
  else if (screen == SCREEN_SOUND) renderSoundPage();
}

// saveAlarm(): Saves alarm time and on/off to flash.
void saveAlarm() {
  prefs.putInt("alH", alarmH);
  prefs.putInt("alM", alarmM);
  prefs.putBool("alOn", alarmOn);
}

// openSettings(): Shows the settings grid without the slide (used when going
//   BACK from a page; the face opens it with slideSettingsOpen).
void openSettings() {
  screen = SCREEN_MENU;
  lastTouchMs = millis();
  renderMenu();
}

// openClockPage(): Opens the Clock page with the current time loaded into the TIME fields.
void openClockPage() {
  struct tm t;
  if (nowLocal(t)) { editH = t.tm_hour; editM = t.tm_min; }
  screen = SCREEN_SETTINGS;
  renderSettings();
}

// pressMenu(): Tile tapped: opens that page (DONE closes settings).
void pressMenu(int id) {
  if (id == B_DONE) { slideSettingsClosed(); return; }
  if (id == B_TILE_CLOCK) { pressedBtn = B_NONE; openClockPage(); return; }
  if (id == B_TILE_SD)    { pressedBtn = B_NONE; openSdPage(); return; }
  if (id == B_TILE_WX)    { pressedBtn = B_NONE; openWxPage(); return; }
  if (id == B_TILE_WIFI)  { pressedBtn = B_NONE; openWifiPage(); return; }
  if (id == B_TILE_BT)    { pressedBtn = B_NONE; openBtPage(); return; }
  if (id == B_TILE_DISP)  { pressedBtn = B_NONE; openDispPage(); return; }
  if (id == B_TILE_SOUND) { pressedBtn = B_NONE; openSoundPage(); return; }
  renderMenu();
}

void openSettings();
void closeSettings();
// ============================================================================
// DISPLAY PAGE: BRIGHTNESS, SLEEP ON BATTERY
// ----------------------------------------------------------------------------
// Settings > Display, plus the brightness rules: full on USB, the chosen
// level on battery.
// ============================================================================
const Btn DISPBTNS[] = {
  { B_BACK, 10, 8, 96, 44 },
  { B_D_BLDN, 22, 94, 60, 52 },  { B_D_BLUP, 238, 94, 60, 52 },
  { B_D_SLPDN, 22, 200, 60, 52 }, { B_D_SLPUP, 238, 200, 60, 52 },
  { B_D_KWIFI, 214, 296, 84, 40 }, { B_D_KBT, 214, 348, 84, 40 },
  { B_D_KMOT, 214, 406, 84, 40 },
};
const int NDISPB = sizeof(DISPBTNS) / sizeof(DISPBTNS[0]);

// drawBrightGlyph(): Half-filled sun (Display tile).
void drawBrightGlyph(int cx, int cy, uint16_t c) {       // sun outline, ~44 px
  frame.drawCircle(cx, cy, 10, c);
  frame.drawCircle(cx, cy, 9, c);
  frame.fillArc(cx, cy, 9, 0, 90, 270, c);               // half filled
  for (int i = 0; i < 8; i++) {
    float a = i * PI / 4;
    frame.drawWideLine(cx + 14 * cosf(a), cy + 14 * sinf(a), cx + 20 * cosf(a), cy + 20 * sinf(a), 1.6f, c);
  }
}

// blPercent(): Battery brightness setting, % (10..100).
int blPercent() { return blPct; }

// pctToPwm(): Brightness % -> backlight PWM value 0..255.
int pctToPwm(int pct) { return constrain(pct, 0, 100) * 255 / 100; }

// drawDispArrow(): Display page buttons: - / + for brightness, < / > for the sleep time.
void drawDispArrow(int id, bool right, bool enabled) {
  const Btn* b = findBtn(DISPBTNS, NDISPB, id);
  frame.fillRoundRect(b->x, b->y, b->w, b->h, 10, pressedBtn == id && enabled ? C_HOUR : C_PANEL);
  int cx = b->x + b->w / 2, cy = b->y + b->h / 2;
  uint16_t c = enabled ? C_LABEL : 0x4208;
  if (id == B_D_BLDN || id == B_D_BLUP) {                // minus / plus
    frame.fillRect(cx - 11, cy - 2, 22, 4, c);
    if (id == B_D_BLUP) frame.fillRect(cx - 2, cy - 11, 4, 22, c);
  } else if (right) frame.fillTriangle(cx - 8, cy - 12, cx - 8, cy + 12, cx + 10, cy, c);
  else frame.fillTriangle(cx + 8, cy - 12, cx + 8, cy + 12, cx - 10, cy, c);
}

// drawSmallToggle(): ON/OFF toggle button on the Display page.
void drawSmallToggle(int id, bool on) {
  const Btn* b = findBtn(DISPBTNS, NDISPB, id);
  drawPageBtn(b->x, b->y, b->w, b->h, id, on ? "ON" : "OFF", on ? C_OK : C_PANEL, on ? TFT_BLACK : TFT_WHITE, true);
}

// renderDispPage(): Settings > Display: brightness on battery, sleep time, what stays on while
//   asleep, and the current power source / battery line.
void renderDispPage() {
  frame.fillScreen(C_BG);
  drawPageHeader("Display", false, false);
  frame.setTextDatum(middle_left);

  // brightness
  frame.drawRoundRect(10, 60, 300, 96, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_LABEL);
  frame.drawString("BRIGHTNESS ON BATTERY", 22, 76);
  frame.setTextDatum(middle_right);
  frame.setTextColor(C_DIM);
  frame.drawString("USB: full", 298, 76);
  frame.setTextDatum(middle_left);
  drawDispArrow(B_D_BLDN, false, blPct > 10);
  drawDispArrow(B_D_BLUP, true, blPct < 100);
  frame.fillRoundRect(92, 112, 136, 16, 6, C_PANEL);
  int w = 136 * blPct / 100;
  if (w > 0) frame.fillRoundRect(92, 112, max(w, 8), 16, 6, C_HOUR);
  char b[16];
  snprintf(b, sizeof(b), "%d%%", blPercent());
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_center);
  frame.drawString(b, 160, 140);

  // sleep on battery
  frame.drawRoundRect(10, 166, 300, 96, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("SLEEP ON BATTERY AFTER", 22, 182);
  drawDispArrow(B_D_SLPDN, false, sleepIdx > 0);
  drawDispArrow(B_D_SLPUP, true, sleepIdx < N_SLEEP - 1);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(sleepIdx ? TFT_WHITE : C_DIM);
  frame.setTextDatum(middle_center);
  frame.drawString(SLEEP_LABELS[sleepIdx], 160, 226);

  // while asleep
  frame.drawRoundRect(10, 272, 300, 128, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("WHILE ASLEEP", 22, 284);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.drawString("Keep Wi-Fi on", 22, 316);
  frame.drawString("Keep Bluetooth on", 22, 368);
  drawSmallToggle(B_D_KWIFI, keepWifiAsleep);
  drawSmallToggle(B_D_KBT, keepBtAsleep);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(TFT_WHITE);
  frame.setTextDatum(middle_left);
  frame.drawString("Wake on tap / move", 22, 426);
  drawSmallToggle(B_D_KMOT, wakeOnMotion);

  // power source
  frame.setFont(&fonts::DejaVu12);
  frame.setTextDatum(middle_center);
  frame.setTextColor(C_DIM);
  if (batPresent) snprintf(b, sizeof(b), "%d%%", batPct);
  String pw = !batPresent ? String("No battery - on USB, never sleeps")
            : usbIn ? "Battery " + String(b) + (batCharging ? ", charging" : "") + " - on USB, no sleep"
            : "Battery " + String(b) + ", " + String(batMv / 1000.0, 2) + " V - on battery";
  frame.drawString(pw, 160, 464);
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openDispPage(): Shows the Display page.
void openDispPage() { screen = SCREEN_DISP; renderDispPage(); }

// pressDisp(): Display page buttons. Brightness changes are shown for 2 s even on USB
//   (where the screen otherwise stays at full) so you can judge the level.
void pressDisp(int id) {
  switch (id) {
    case B_BACK: pressedBtn = B_NONE; openSettings(); return;
    case B_D_BLDN: blPct = max(10, blPct - 10); break;              // 10% steps
    case B_D_BLUP: blPct = min(100, blPct + 10); break;
    case B_D_SLPDN: if (sleepIdx > 0) sleepIdx--; prefs.putInt("sleepI", sleepIdx); break;
    case B_D_SLPUP: if (sleepIdx < N_SLEEP - 1) sleepIdx++; prefs.putInt("sleepI", sleepIdx); break;
    case B_D_KWIFI: keepWifiAsleep = !keepWifiAsleep; prefs.putBool("slpWifi", keepWifiAsleep); break;
    case B_D_KBT: keepBtAsleep = !keepBtAsleep; prefs.putBool("slpBt", keepBtAsleep); break;
    case B_D_KMOT: wakeOnMotion = !wakeOnMotion; prefs.putBool("wakeMov", wakeOnMotion); break;
  }
  if (id == B_D_BLDN || id == B_D_BLUP) {  // show the battery level for 2 s (saved on release)
    lcd.setBrightness(pctToPwm(blPct));
    blPreviewUntil = millis() + 2000;
    if (batPresent && !usbIn) backlight = pctToPwm(blPct);
  }
  renderDispPage();
}

// brightness for the power source: full on USB, the chosen level on battery
int targetBacklight() { return (batPresent && !usbIn) ? pctToPwm(blPct) : 255; }
void fadeBacklight(int from, int to, int ms);

// applyBacklight(): Moves the backlight to the level for the current power source
//   (optionally with a short fade).
void applyBacklight(bool fade) {
  int t = targetBacklight();
  if (t == backlight) return;
  if (fade) fadeBacklight(backlight, t, 400); else lcd.setBrightness(t);
  backlight = t;
}

// ============================================================================
// SOUND PAGE + HOURLY CHIME
// ----------------------------------------------------------------------------
// Settings > Sound: the hourly chime (on/off, its own volume, TEST, the sound)
// and the quiet hours. The chime rings on the hour (hh:00) unless it's in the
// quiet hours or the alarm rings that same minute. Sound: chimeFile on the SD
// card if one is chosen (played once by the audio library), otherwise the
// built-in two-note bell (played by the beep task). The codec volume is set to
// the chime's level while it plays, then back to the alarm's.
// ============================================================================
bool chimeActive = false;                // a chime (or TEST / volume preview) is playing
unsigned long chimeStartMs = 0;
unsigned long chimeVolChangedAt = 0;     // volume tapped: play the chime at the new level

// quietHour(): true if hour h is inside the quiet hours (FROM up to, not
//   including, TO; may wrap past midnight; FROM == TO = no quiet hours).
bool quietHour(int h) {
  if (!quietOn || quietFrom == quietTo) return false;
  return quietFrom < quietTo ? (h >= quietFrom && h < quietTo) : (h >= quietFrom || h < quietTo);
}

// inQuietHours(): quiet right now? (the face's chime icon goes hollow)
bool inQuietHours() {
  struct tm t;
  return nowLocal(t) && quietHour(t.tm_hour);
}

// startChime(): Plays the chime once at chimeVol: the SD file if one is set
//   and playable, else the bell. Nothing while the alarm rings.
void startChime() {
  if (!audioOk || alarmRinging || chimeActive || chimeVol == 0) return;
  previewBeeps = 0;
  setAlarmVolume(chimeVol);              // (main task: I2C)
  ampOn(true);
  chimeActive = true;
  chimeStartMs = millis();
  if (!startChimeMusic()) chimeBell = true;   // the beep task plays the bell
}

// endChime(): Stops the chime (finished, TEST pressed again, or the alarm is
//   starting), switches the amp off and gives the codec back the alarm volume.
void endChime() {
  if (!chimeActive) return;
  chimeBell = false;                     // the beep task stops within 10 ms
  stopChimeMusic();
  chimeActive = false;
  delay(30);                             // let the last buffer play out
  if (!alarmRinging && !previewAmpUntil) ampOn(false);
  setAlarmVolume(alarmVol);
}

// chimeService(): Called every loop(): rings on the hour, ends a chime that has
//   played out (20 s at most), and plays the volume preview.
void chimeService() {
  struct tm t;
  if (chimeOn && !demoActive && nowLocal(t) && t.tm_min == 0 && t.tm_sec < 30) {
    long key = t.tm_yday * 24L + t.tm_hour;
    if (key != lastChimeKey) {
      lastChimeKey = key;
      bool alarmNow = alarmOn && alarmH == t.tm_hour && alarmM == 0;   // the alarm wins
      if (!quietHour(t.tm_hour) && !alarmNow && !alarmRinging) {
        Serial.printf("Chime %02d:00\n", t.tm_hour);
        startChime();
      }
    }
  }
  if (chimeActive && ((!chimeBell && !chimeMusic) || millis() - chimeStartMs > 20000)) endChime();
  if (chimeVolChangedAt && millis() - chimeVolChangedAt > 600 && !alarmRinging) {
    chimeVolChangedAt = 0;
    endChime();
    startChime();
  }
}

const Btn SNDBTNS[] = {
  { B_BACK, 10, 8, 96, 44 },
  { B_C_ON, 222, 66, 76, 40 },
  { B_C_VDN, 22, 146, 56, 44 }, { B_C_VUP, 158, 146, 56, 44 }, { B_C_TEST, 228, 146, 70, 44 },
  { B_C_SND, 10, 204, 300, 42 },         // the SOUND row (tap: pick a file)
  { B_Q_ON, 222, 268, 76, 40 },
  { B_QF_UP, 45, 326, 70, 36 }, { B_QF_DN, 45, 416, 70, 36 },
  { B_QT_UP, 205, 326, 70, 36 }, { B_QT_DN, 205, 416, 70, 36 },
};
const int NSNDB = sizeof(SNDBTNS) / sizeof(SNDBTNS[0]);

// drawSndToggle(): ON/OFF button on the Sound page.
void drawSndToggle(int id, bool on) {
  const Btn* b = findBtn(SNDBTNS, NSNDB, id);
  drawPageBtn(b->x, b->y, b->w, b->h, id, on ? "ON" : "OFF", on ? C_OK : C_PANEL, on ? TFT_BLACK : TFT_WHITE, true);
}

// renderSoundPage(): Settings > Sound - HOURLY CHIME panel (ON/OFF, volume - / +,
//   TEST, sound) and QUIET HOURS panel (ON/OFF, FROM and TO hour with arrows).
void renderSoundPage() {
  frame.fillScreen(C_BG);
  drawPageHeader("Sound", false, false);

  // hourly chime
  frame.drawRoundRect(10, 60, 300, 190, 12, C_WINRIM);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(C_LABEL);
  frame.setTextDatum(middle_left);
  frame.drawString("HOURLY CHIME", 22, 86);
  drawSndToggle(B_C_ON, chimeOn);
  const Btn* b = findBtn(SNDBTNS, NSNDB, B_C_VDN);
  drawPageBtn(b->x, b->y, b->w, b->h, B_C_VDN, "-", C_PANEL, TFT_WHITE, true);
  b = findBtn(SNDBTNS, NSNDB, B_C_VUP);
  drawPageBtn(b->x, b->y, b->w, b->h, B_C_VUP, "+", C_PANEL, TFT_WHITE, true);
  b = findBtn(SNDBTNS, NSNDB, B_C_TEST);
  drawPageBtn(b->x, b->y, b->w, b->h, B_C_TEST, chimeActive ? "STOP" : "TEST", chimeActive ? C_HOUR : C_PANEL,
              chimeActive ? TFT_BLACK : TFT_WHITE, true);
  char vb[8];
  snprintf(vb, sizeof(vb), "%d%%", chimeVol);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextDatum(middle_center);
  frame.setTextColor(chimeVol == 0 ? C_BAD : chimeOn ? TFT_WHITE : C_DIM);
  frame.drawString(chimeVol ? vb : "MUTE", 118, 169);
  frame.setFont(&fonts::DejaVu12);
  frame.setTextDatum(middle_left);
  frame.setTextColor(C_LABEL);
  frame.drawString("SOUND", 22, 226);
  String nm = chimeFile.length() ? chimeFile.substring(chimeFile.lastIndexOf('/') + 1) : String("bell (built-in)");
  frame.setTextDatum(middle_right);
  frame.setTextColor(pressedBtn == B_C_SND ? C_HOUR : C_DATE);
  frame.drawString(fitText(nm, 200, false) + "  >", 298, 226);

  // quiet hours - greyed out (and not tappable) while the chime is off
  const uint16_t GREY = 0x4208;
  bool act = chimeOn;
  uint16_t lab = act ? C_LABEL : GREY;
  frame.drawRoundRect(10, 262, 300, 206, 12, act ? C_WINRIM : 0x2945);
  frame.setFont(&fonts::DejaVu18);
  frame.setTextColor(lab);
  frame.setTextDatum(middle_left);
  frame.drawString("QUIET HOURS", 22, 288);
  {
    const Btn* q = findBtn(SNDBTNS, NSNDB, B_Q_ON);
    if (act) drawSndToggle(B_Q_ON, quietOn);
    else drawPageBtn(q->x, q->y, q->w, q->h, B_Q_ON, quietOn ? "ON" : "OFF", 0x2104, GREY, true);
  }
  frame.setFont(&fonts::DejaVu12);
  frame.setTextColor(lab);               // (the toggle left its own text colour set)
  frame.setTextDatum(middle_center);
  frame.drawString("FROM", 80, 318);
  frame.drawString("TO", 240, 318);
  const int arr[4] = { B_QF_UP, B_QF_DN, B_QT_UP, B_QT_DN };
  for (int i = 0; i < 4; i++)
    drawArrowIn(findBtn(SNDBTNS, NSNDB, arr[i]), i % 2 == 0, act && pressedBtn == arr[i] ? C_HOUR : lab);
  char hb[4];
  uint16_t qc = !act ? GREY : quietOn ? C_HOUR : 0x8410;
  frame.setFont(&fonts::DejaVu40);
  frame.setTextColor(qc);
  snprintf(hb, sizeof(hb), "%02d", quietFrom); frame.drawString(hb, 80, 390);
  snprintf(hb, sizeof(hb), "%02d", quietTo);   frame.drawString(hb, 240, 390);
  frame.setFont(&fonts::DejaVu24);
  frame.setTextColor(lab);
  frame.drawString("->", 160, 390);
  frame.setFont(&fonts::DejaVu12);
  if (millis() < savedMsgUntil) {
    frame.setTextColor(savedMsgColor);
    frame.drawString(savedMsg, 160, 459);
  } else {
    char q[40];
    if (!act) strcpy(q, "chime is off");
    else if (!quietOn) strcpy(q, "chimes every hour");
    else if (quietFrom == quietTo) strcpy(q, "FROM = TO: no quiet hours");
    else snprintf(q, sizeof(q), "no chime %02d:00 - %02d:59", quietFrom, (quietTo + 23) % 24);
    frame.setTextColor(act ? C_DIM : GREY);
    frame.drawString(q, 160, 459);
  }
  if (alarmRinging) drawRinging();
  frame.pushSprite(0, 0);
}

// openSoundPage(): Shows the Sound page.
void openSoundPage() { screen = SCREEN_SOUND; lastTouchMs = millis(); renderSoundPage(); }

// saveSound(): Volume and quiet hours are saved when the finger lifts
//   (not on every auto-repeat step).
void saveSound() {
  prefs.putInt("chVol", chimeVol);
  prefs.putInt("chQF", quietFrom);
  prefs.putInt("chQT", quietTo);
}

// pressSound(): Sound page buttons. Arrows move 1 hour; volume - / + move 5%.
void pressSound(int id) {
  bool quietCtl = id == B_Q_ON || id == B_QF_UP || id == B_QF_DN || id == B_QT_UP || id == B_QT_DN;
  if (quietCtl && !chimeOn) { pressedBtn = B_NONE; return; }   // greyed out while the chime is off
  switch (id) {
    case B_BACK: saveSound(); pressedBtn = B_NONE; openSettings(); return;
    case B_C_ON: chimeOn = !chimeOn; prefs.putBool("chOn", chimeOn); break;
    case B_C_VDN: chimeVol = max(0, (chimeVol + 4) / 5 * 5 - 5); chimeVolChangedAt = millis(); break;
    case B_C_VUP: chimeVol = min(100, chimeVol / 5 * 5 + 5); chimeVolChangedAt = millis(); break;
    case B_C_TEST: if (chimeActive) endChime(); else startChime(); break;
    case B_C_SND: saveSound(); endChime(); pressedBtn = B_NONE; openSoundPicker(true); return;
    case B_Q_ON: quietOn = !quietOn; prefs.putBool("chQt", quietOn); break;
    case B_QF_UP: quietFrom = (quietFrom + 1) % 24; break;
    case B_QF_DN: quietFrom = (quietFrom + 23) % 24; break;
    case B_QT_UP: quietTo = (quietTo + 1) % 24; break;
    case B_QT_DN: quietTo = (quietTo + 23) % 24; break;
  }
  renderSoundPage();
}

// ============================================================================
// SLEEP / WAKE WITH A BACKLIGHT FADE
// ----------------------------------------------------------------------------
// Sleep is only for battery power: the backlight fades out, drawing stops,
// optional radios go off. Touch, the alarm or plugging in USB wakes it.
// ============================================================================
// fade in perceived brightness (gamma 2.2) so it looks even
void fadeBacklight(int from, int to, int ms) {
  const int steps = max(1, ms / 15);
  float a = powf(from / 255.0f, 1 / 2.2f), z = powf(to / 255.0f, 1 / 2.2f);
  for (int i = 1; i <= steps; i++) {
    float t = (float)i / steps;
    float e = t * t * (3 - 2 * t);                       // ease in-out
    float g = a + (z - a) * e;
    lcd.setBrightness((uint8_t)lroundf(powf(g, 2.2f) * 255));
    delay(15);
  }
  lcd.setBrightness(to);
}

void updateClock(bool force);

// goSleep(): Battery sleep: back to the clock face, fade the backlight out, and switch
//   off Wi-Fi / Bluetooth unless set to stay on. Drawing stops while asleep.
bool imuRead();
void goSleep() {
  if (screen != SCREEN_FACE) closeSettings();
  fadeBacklight(backlight, 0, 700);
  sleeping = true;
  if (imuOk && imuRead()) { motRefX = accX; motRefY = accY; motRefZ = accZ; }   // "still" position
  sleptWifi = sleptBt = false;
  if (wifiOn && !keepWifiAsleep) { WiFi.disconnect(true); WiFi.mode(WIFI_OFF); sleptWifi = true; }
  if (btOn && !keepBtAsleep) { bleStop(); sleptBt = true; }
  // save power while dark: display controller into its sleep mode, and the
  // processor down from 240 to 80 MHz (Wi-Fi, SPI, I2C, I2S and USB all
  // still work at 80; the loop just has less to do)
  lcd.sleep();
  Serial.println("Sleep");
  Serial.flush();
  setCpuFrequencyMhz(80);
  statSleepStart = millis();
}

// wakeUp(): Wake: draw the current face while still dark, fade in, and switch the radios
//   back on if sleep switched them off.
void wakeUp() {
  setCpuFrequencyMhz(240);               // full speed again
  statAsleepMs += millis() - statSleepStart;
  sleeping = false;
  lcd.wakeup();                          // display controller out of sleep mode
  lcd.setBrightness(0);                  //   (keep it dark until the fade)
  delay(120);                            //   the panel needs 120 ms after waking
  shownHour = -1;
  updateClock(true);                     // draw the current face while still dark
  backlight = targetBacklight();         // the power source may have changed
  fadeBacklight(0, backlight, 500);
  if (sleptWifi) { WiFi.mode(WIFI_STA); wifiStart(); }
  if (sleptBt) bleStart();
  sleptWifi = sleptBt = false;
  lastTouchMs = millis();
  Serial.println("Wake");
}

// pressWifi(): Wi-Fi page buttons: BACK, ON/OFF, scroll, SCAN.
void pressWifi(int id) {
  int n = wifiNets.size();
  switch (id) {
    case B_BACK: pressedBtn = B_NONE; openSettings(); return;
    case B_TOGGLE: setWifi(!wifiOn); break;
    case B_W_UP: wifiScroll = max(0, wifiScroll - WL_ROWS); break;
    case B_W_DN: if (wifiScroll + WL_ROWS < n) wifiScroll += WL_ROWS; break;
    case B_W_SCAN: wifiScanStart(); break;
  }
  renderWifiPage();
}

// wifiTapRow(): Tapped a network: open networks join directly, locked ones open the keyboard.
void wifiTapRow(int ty) {
  int row = (ty - WL_ROW_Y) / WL_ROW_H, idx = wifiScroll + row;
  if (!wifiOn || row < 0 || row >= WL_ROWS || idx >= (int)wifiNets.size()) return;
  const WifiNet& w = wifiNets[idx];
  if (w.secure) openKbd(w.ssid);
  else joinWifi(w.ssid, "");
}

// pressBt(): Bluetooth page buttons.
void pressBt(int id) {
  switch (id) {
    case B_BACK: pressedBtn = B_NONE; openSettings(); return;
    case B_TOGGLE: setBt(!btOn); break;
    case B_BT_NEWPIN: newBtPin(); sdNote("New PIN - pair the phone again", C_HOUR); break;
    case B_BT_FORGET:
      if (bleInited) NimBLEDevice::deleteAllBonds();
      bleDisconnectAll();
      sdNote("Paired phones forgotten", C_HOUR);
      break;
    case B_BT_DISC: bleDisconnectAll(); break;
  }
  renderBtPage();
}

// pressWx(): Weather keypad: digits (max 5), DEL, OK (hands the ZIP to the network task).
void pressWx(int id) {
  if (id == B_BACK) { pressedBtn = B_NONE; openSettings(); return; }
  if (id >= B_K0 && id <= B_K9 && zipEntry.length() < 5) { zipEntry += (char)('0' + (id - B_K0)); if (wxLookup != 1) wxLookup = 0; }
  else if (id == B_KDEL && zipEntry.length()) { zipEntry.remove(zipEntry.length() - 1); if (wxLookup != 1) wxLookup = 0; }
  else if (id == B_KOK && zipEntry.length() == 5 && wxLookup != 1) {
    strlcpy(wxQuery, zipEntry.c_str(), sizeof(wxQuery));
    wxLookup = 1;                        // the network task picks it up
  }
  renderWxPage();
}

// pressSd(): SD page buttons, including picker-mode BACK (to the Clock page) and BEEP.
void pressSd(int id) {
  if (sdConfirm) {
    if (id == B_SD_CANCEL) sdConfirm = false;
    else if (id == B_SD_YES) { sdConfirm = false; sdFormat(); }
    renderSd();
    return;
  }
  int n = sdRowCount();
  switch (id) {
    case B_BACK:
      sdUnmount(); pressedBtn = B_NONE;
      if (sdPick) { sdPick = false; if (sdPickChime) openSoundPage(); else openClockPage(); } else openSettings();
      return;
    case B_SD_USB: if (sdPick || sdState == 2) break; pressedBtn = B_NONE; openUsbPage(); return;
    case B_SD_UP: sdScroll = max(0, sdScroll - SD_ROWS); break;
    case B_SD_DN: if (sdScroll + SD_ROWS < n) sdScroll += SD_ROWS; break;
    case B_SD_REFRESH: sdRefresh(); break;
    case B_SD_FORMAT:
      if (sdPick) {                      // "BEEP" / "BELL": back to the built-in sound
        sdUnmount(); sdPick = false; pressedBtn = B_NONE;
        if (sdPickChime) {
          chimeFile = ""; prefs.putString("chSnd", "");
          openSoundPage();
          savedMsg = "Chime sound: bell";
        } else {
          alarmFile = ""; prefs.putString("alSnd", "");
          openClockPage();
          savedMsg = "Alarm sound: beep";
        }
        savedMsgColor = C_OK; savedMsgUntil = millis() + 1500;
        return;
      }
      if (sdState == 1 || sdState == 3) sdConfirm = true;
      break;
  }
  renderSd();
}

// closeSettings(): Leaves settings: saves the alarm, unmounts the card, back to the face.
void saveSound();
void closeSettings() {
  saveAlarm();
  if (screen == SCREEN_SOUND) saveSound();
  sdUnmount();
  pressedBtn = B_NONE;
  screen = SCREEN_FACE;
  shownHour = -1;
  updateClock(true);
}

// setClockTo(): SET on the Clock page: sets the system clock and the RTC to HH:MM today.
//   The next NTP sync will correct it if Wi-Fi is available.
void setClockTo(int h, int m) {
  struct tm t;
  if (!nowLocal(t)) { memset(&t, 0, sizeof(t)); t.tm_year = 126; t.tm_mon = 0; t.tm_mday = 1; }
  t.tm_hour = h; t.tm_min = m; t.tm_sec = 0; t.tm_isdst = -1;
  time_t nt = mktime(&t);
  struct timeval tv = { nt, 0 };
  settimeofday(&tv, nullptr);
  if (rtcFound) rtcWrite(nt);
  Serial.printf("Time set to %02d:%02d (the next NTP sync will correct it)\n", h, m);
}

// isArrow(): Buttons that auto-repeat while held.
bool isArrow(int id) {
  return id == B_TH_UP || id == B_TH_DN || id == B_TM_UP || id == B_TM_DN ||
         id == B_AH_UP || id == B_AH_DN || id == B_AM_UP || id == B_AM_DN ||
         id == B_N_UP || id == B_N_DN || id == B_VDN || id == B_VUP ||
         id == B_C_VDN || id == B_C_VUP || id == B_QF_UP || id == B_QF_DN || id == B_QT_UP || id == B_QT_DN;
}

// pressSettings(): Clock page buttons.
void pressSettings(int id) {
  switch (id) {
    case B_TH_UP: editH = (editH + 1) % 24; break;
    case B_TH_DN: editH = (editH + 23) % 24; break;
    case B_TM_UP: editM = (editM + 1) % 60; break;
    case B_TM_DN: editM = (editM + 59) % 60; break;
    case B_AH_UP: alarmH = (alarmH + 1) % 24; break;
    case B_AH_DN: alarmH = (alarmH + 23) % 24; break;
    case B_AM_UP: alarmM = (alarmM + 1) % 60; break;
    case B_AM_DN: alarmM = (alarmM + 59) % 60; break;
    case B_SET: setClockTo(editH, editM); savedMsg = "Time set"; savedMsgColor = C_OK; savedMsgUntil = millis() + 1500; break;
    case B_WSYNC:
      if (WiFi.status() != WL_CONNECTED) {
        savedMsg = "No Wi-Fi - can't sync"; savedMsgColor = C_BAD;
      } else {
        sntp_restart();                  // the result arrives in loop() (sntpDone)
        clockSyncAsked = true;
        savedMsg = "Syncing..."; savedMsgColor = C_HOUR;
      }
      savedMsgUntil = millis() + 4000;
      break;
    case B_AON:
      alarmOn = !alarmOn;
      prefs.putBool("alOn", alarmOn);
      savedMsg = alarmOn ? "Alarm on" : "Alarm off"; savedMsgColor = C_OK; savedMsgUntil = millis() + 1500;
      break;
    case B_TEST: if (alarmRinging) stopAlarm(); else ringClockAlarm(true); break;
    case B_VDN: case B_VUP:
      alarmVol = constrain(alarmVol + (id == B_VUP ? 10 : -10), 0, 100);
      setAlarmVolume(alarmVol);
      prefs.putInt("alVol", alarmVol);
      volChangedAt = millis();           // three beeps once you stop tapping
      break;
    case B_SOUND: pressedBtn = B_NONE; openSoundPicker(false); return;
    case B_FACE:                         // next clock face
      faceStyle = (faceStyle + 1) % N_FACES;
      prefs.putInt("face", faceStyle);
      break;
    case B_BACK: saveAlarm(); pressedBtn = B_NONE; openSettings(); return;
  }
  renderSettings();
}

// ============================================================================
// NEST PANEL TOUCHES
// ----------------------------------------------------------------------------
// Taps on the Nest panel in the bottom half.
// ============================================================================
void nestNote(const char* m) {
  strlcpy(nestMsg, m, sizeof(nestMsg));
  nestMsgUntil = millis() + 2500;
}

// pressNest(): Nest panel taps. Up/down change a pending target (sent 1.5 s after the
//   last tap, so a burst of taps is one command); mode buttons are sent at once.
//   Leaving ECO has to happen before another mode can be chosen.
void pressNest(int id) {
  NestState s;
  xSemaphoreTake(nestLock, portMAX_DELAY);
  s = nest;
  xSemaphoreGive(nestLock);
  if (!s.valid) return;
  bool eco = !strcmp(s.eco, "MANUAL_ECO");
  if (id == B_N_UP || id == B_N_DN) {
    if (eco || !strcmp(s.mode, "OFF")) { nestNote(eco ? "turn ECO off to set a temperature" : "pick a mode first"); return; }
    bool showingSent = sentShow && nestVersion == sentVersion;
    if (!pendingSet && !showingSent) { pendHeatC = s.heatC; pendCoolC = s.coolC; }
    sentShow = false;
    // one step = 1 F (or 0.5 C), in the thermostat's own unit
    float step = s.scaleF ? 5.0f / 9.0f : 0.5f;
    float d = (id == B_N_UP) ? step : -step;
    if (!strcmp(s.mode, "HEAT")) pendHeatC += d;
    else if (!strcmp(s.mode, "COOL")) pendCoolC += d;
    else { pendHeatC += d; pendCoolC += d; }   // AUTO: move the whole range
    pendHeatC = constrain(pendHeatC, 9.0f, 32.0f);
    pendCoolC = constrain(pendCoolC, 9.0f, 32.0f);
    pendingSet = true;
    pendLastTap = millis();
    return;
  }
  const char* m = nullptr;
  if (id == B_M_HEAT) m = "HEAT";
  else if (id == B_M_COOL) m = "COOL";
  else if (id == B_M_AUTO) m = "HEATCOOL";
  else if (id == B_M_OFF) m = "OFF";
  if (m) {
    if (eco) { strlcpy(cmdMode, "OFF", sizeof(cmdMode)); cmdType = 3; }   // leave ECO first
    else { strlcpy(cmdMode, m, sizeof(cmdMode)); cmdType = 2; }
    if (eco) nestNote("ECO off - tap the mode again");
    else nestNote("sending...");
  } else if (id == B_M_ECO) {
    strlcpy(cmdMode, eco ? "OFF" : "MANUAL_ECO", sizeof(cmdMode));
    cmdType = 3;
    nestNote(eco ? "ECO off..." : "ECO on...");
  }
}

// ============================================================================
// THE CLOCK
// ----------------------------------------------------------------------------
// Turns the current time into a frame: hand angle, seconds, hour digits,
// and the hour-change animation.
// ============================================================================
float handDegFor(const struct tm& t) { return minuteToDeg(t.tm_min + t.tm_sec / 60.0f); }

// hourChange(): The top-of-the-hour animation: the hand flies back from 60 to 00 (ease-out),
//   then the hour digits roll (smoothstep). About 32 frames.
void hourChange(const struct tm& t, int oldHour) {
  float from = minuteToDeg(60), to = minuteToDeg(0);
  for (int f = 1; f <= 18; f++) {
    float u = f / 18.0f;
    float e = 1 - (1 - u) * (1 - u) * (1 - u);
    render(from + (to - from) * e, -1, oldHour, oldHour, 0, true, t);
  }
  for (int f = 1; f <= 14; f++) {
    float u = f / 14.0f;
    float e = u * u * (3 - 2 * u);
    render(to, -1, oldHour, t.tm_hour, e, true, t);
  }
}

// updateClock(): Redraws the face when the second changes (or when forced). Detects the hour
//   change and plays its animation first. Does nothing while a settings page
//   owns the screen.
void updateClock(bool force) {
  if (screen != SCREEN_FACE) return;     // settings pages own the screen
  struct tm t = {};
  bool known = nowLocal(t);
  if (!known) {
    if (force || shownSec != -2) { render(minuteToDeg(0), -1, 0, 0, 0, false, t); shownSec = -2; }
    return;
  }
  if (!force && t.tm_sec == shownSec) return;
  if (shownHour >= 0 && t.tm_hour != shownHour && t.tm_min == 0 && !force && faceStyle == FACE_JUMP)
    hourChange(t, shownHour);           // (the fly-back + roll animation is the jump face's)
  shownHour = t.tm_hour;
  shownSec = t.tm_sec;
  shownHand = handDegFor(t);
  render(shownHand, t.tm_sec, t.tm_hour, t.tm_hour, 0, true, t);
}

// ============================================================================
// ORIENTATION (QMI8658 ACCELEROMETER)
// ----------------------------------------------------------------------------
// The motion sensor tells which way gravity points. On this board the
// sensor's X axis runs top-to-bottom (negative when upright) and Y runs
// left-to-right.
// ============================================================================
bool imuInit() {
  uint8_t id = 0;
  if (i2cRead(IMU_ADDR, 0x00, &id, 1) != 1 || id != 0x05) { Serial.printf("IMU not found (id 0x%02X)\n", id); return false; }
  i2cWrite(IMU_ADDR, 0x60, 0xB0);        // soft reset
  delay(20);
  i2cWrite(IMU_ADDR, 0x02, 0x40);        // CTRL1: register auto-increment, little endian
  i2cWrite(IMU_ADDR, 0x03, 0x05);        // CTRL2: accel +-2 g, 250 Hz (fast enough to catch a tap)
  i2cWrite(IMU_ADDR, 0x08, 0x01);        // CTRL7: accelerometer on
  return true;
}

// imuRead(): Reads the accelerometer (x, y, z in g). +-2 g range -> 16384 counts per g.
bool imuRead() {
  uint8_t d[6];
  if (i2cRead(IMU_ADDR, 0x35, d, 6) != 6) return false;
  accX = (int16_t)(d[1] << 8 | d[0]) / 16384.0f;
  accY = (int16_t)(d[3] << 8 | d[2]) / 16384.0f;
  accZ = (int16_t)(d[5] << 8 | d[4]) / 16384.0f;
  return true;
}

// which way is up?  -1 = can't tell (lying flat / moving)
// On this board the sensor's X axis runs top-bottom (X < 0 when upright),
// Y runs left-right.
bool turnedLeft() { return (accY < 0) != orientFlip; }

// orientFromAccel(): Which layout the board's position asks for: 0 upright, 3 upside down,
//   2 turned left (clock), 1 turned right (weather); -1 = lying flat / unclear.
//   The 0.65 g / 0.25 g margins stop it flickering between two positions.
int orientFromAccel() {
  float ax = fabsf(accX), ay = fabsf(accY);
  if (ax > 0.65f && ax > ay + 0.25f) return accX < 0 ? 0 : 3;        // upright / upside down
  if (ay > 0.65f && ay > ax + 0.25f) return (turnedLeft() != orientSwap) ? 2 : 1;
  return -1;
}

// applyOrientation(): Switches layout: sets the frame size, clock geometry and display rotation,
//   re-creates the PSRAM frame at the new size, and redraws.
void applyOrientation(int o) {
  if (o == orient) return;
  orient = o;
  bool land = isSideways();
  W = land ? 480 : 320;
  H = land ? 320 : 480;
  setClockGeometry(o == 2);
  // picture direction: left turn = rotation 1, right = 3, upside down = 2
  lcd.setRotation(o == 0 ? 0 : o == 3 ? 2 : (turnedLeft() ? 1 : 3));
  frame.deleteSprite();
  if (!frame.createSprite(W, H)) Serial.println("frame re-create FAILED");
  pressedBtn = B_NONE;
  shownHour = -1;
  Serial.printf("Orientation: %s\n", o == 0 ? "upright" : o == 3 ? "upside down" : o == 1 ? "sideways - weather" : "sideways - clock");
  updateClock(true);
}

// orientService(): Called every loop(): reads the sensor every 200 ms and changes layout only
//   after the new position has been steady for 0.8 s (and only on the face,
//   not while asleep).
void orientService() {                    // loop(): every 200 ms, change after 0.8 s steady
  static unsigned long last = 0, since = 0;
  static int cand = -1;
  if (!imuOk || millis() - last < 200) return;
  last = millis();
  if (!imuRead()) return;
  int o = orientAuto ? orientFromAccel() : 0;
  if (o < 0 || o == orient) { cand = -1; return; }
  if (o != cand) { cand = o; since = millis(); return; }
  if (millis() - since < 800) return;
  if (screen != SCREEN_FACE || sleeping) return;   // only from the clock face, awake
  cand = -1;
  applyOrientation(o);
}

// ============================================================================
// COMMANDS
// ----------------------------------------------------------------------------
// Text commands from the Serial Monitor (115200, Newline) or a paired phone.
// ============================================================================
String masked(const String& s) {         // show only the start of a secret
  if (s.length() == 0) return "(not set)";
  return s.substring(0, min((int)s.length(), 6)) + "... (" + String(s.length()) + " chars)";
}

// handleCommand(): Runs one text command from the Serial Monitor or a paired phone. Replies go
//   through 'Out', which copies them to both. Secrets are stored but never echoed.
void handleCommand(String in) {
  in.trim();
  String lower = in; lower.toLowerCase();
  if (lower.startsWith("ssid ")) {
    wifiSsid = in.substring(5); wifiSsid.trim();
    prefs.putString("ssid", wifiSsid);
    Out.println("Wi-Fi name saved: " + wifiSsid + " - now send: pass <password>");
  } else if (lower.startsWith("pass ")) {
    wifiPass = in.substring(5);
    prefs.putString("pass", wifiPass);
    Out.printf("Password saved (%d characters). Connecting...\n", wifiPass.length());
    if (!wifiOn) { wifiOn = true; prefs.putBool("wifiOn", true); }
    WiFi.disconnect();
    wifiStart();
  } else if (lower.startsWith("orient")) {
    String a = lower.substring(6); a.trim();
    imuRead();
    if (a == "flip") { orientFlip = !orientFlip; prefs.putBool("orFlip2", orientFlip); }
    else if (a == "swap") { orientSwap = !orientSwap; prefs.putBool("orSwap2", orientSwap); }
    else if (a == "off") { orientAuto = false; prefs.putBool("orAuto", false); applyOrientation(0); }
    else if (a == "on") { orientAuto = true; prefs.putBool("orAuto", true); }
    if ((a == "flip" || a == "swap") && isSideways() && screen == SCREEN_FACE) {
      int o = orientFromAccel();           // redraw the sideways screen with the new setting
      orient = -1;
      applyOrientation(o < 0 ? 0 : o);
    }
    Out.printf("Orientation: %s (auto %s, flip %s, swap %s), accel x %.2f y %.2f z %.2f g%s\n",
               orient == 0 ? "upright" : orient == 3 ? "upside down" : orient == 1 ? "sideways - weather" : "sideways - clock",
               orientAuto ? "on" : "off", orientFlip ? "on" : "off", orientSwap ? "on" : "off",
               accX, accY, accZ, imuOk ? "" : " (sensor not found)");
  } else if (lower == "wifi on" || lower == "wifi off") {
    setWifi(lower == "wifi on");
    Out.println(wifiOn ? "Wi-Fi on" : "Wi-Fi off");
  } else if (lower == "bt on" || lower == "bt off") {
    setBt(lower == "bt on");
  } else if (lower == "bt") {
    Out.printf("Bluetooth: %s, PIN %06u, %s\n", btOn ? "on" : "off", (unsigned)btPin,
                  bleSecure ? "phone paired" : (bleConnected ? "phone connected (not paired)" : "no phone"));
  } else if (lower == "sync") {
    sntp_restart();
    Out.println("NTP sync requested");
  } else if (lower.startsWith("nest ")) {
    String rest = in.substring(5); rest.trim();
    int sp = rest.indexOf(' ');
    String key = (sp < 0 ? rest : rest.substring(0, sp)); key.toLowerCase();
    String val = sp < 0 ? String() : rest.substring(sp + 1); val.trim();
    if (key == "project" && val.length()) { nestProject = val; prefs.putString("nProj", val); Out.println("Nest project saved"); }
    else if (key == "client" && val.length()) { nestClient = val; prefs.putString("nCli", val); Out.println("Nest client ID saved"); }
    else if (key == "secret" && val.length()) { nestSecret = val; prefs.putString("nSec", val); Out.println("Nest client secret saved"); }
    else if (key == "token" && val.length()) {
      nestRefresh = val; prefs.putString("nRef", val);
      accessToken = ""; nestPollSoon = true;
      Out.println("Nest refresh token saved - connecting to Google...");
    }
    else if (key == "clear") {
      prefs.remove("nProj"); prefs.remove("nCli"); prefs.remove("nSec"); prefs.remove("nRef");
      nestProject = nestClient = nestSecret = nestRefresh = accessToken = "";
      Out.println("Nest credentials erased");
    }
    else if (key == "poll") { nestPollSoon = true; Out.println("Nest: reading now"); }
    else if (key == "status") {
      Out.println("Nest project: " + masked(nestProject));
      Out.println("Nest client:  " + masked(nestClient));
      Out.println("Nest secret:  " + String(nestSecret.length() ? "set" : "(not set)"));
      Out.println("Nest token:   " + String(nestRefresh.length() ? "set" : "(not set)"));
      NestState s; xSemaphoreTake(nestLock, portMAX_DELAY); s = nest; xSemaphoreGive(nestLock);
      if (s.valid) Out.printf("Thermostat: %.1f C, %d%%, mode %s, eco %s, hvac %s, heat %.1f, cool %.1f, %s, read %lus ago\n",
                                 s.ambientC, s.humidity, s.mode, s.eco, s.hvac, s.heatC, s.coolC,
                                 s.scaleF ? "F" : "C", (millis() - s.updatedMs) / 1000);
      if (s.error[0]) Out.println(String("Last error: ") + s.error);
    }
    else Out.println("nest project|client|secret|token <value> | nest status | nest poll | nest clear");
  } else if (lower == "sd" || lower.startsWith("sd ")) {
    String path = in.substring(2); path.trim();
    if (path.length() == 0) path = "/";
    if (!path.startsWith("/")) path = "/" + path;
    bool was = sdMounted;
    sdMount(false);
    if (!sdMounted) Out.println(sdState == 3 ? "SD: card found but not FAT formatted" : "SD: no card");
    else {
      Out.printf("SD: %s %.8s, %s total, %s free\n", sdCardType(), sdCard->cid.name,
                    fmtBytes(sdTotal).c_str(), fmtBytes(sdFree).c_str());
      String full = sdFull(path);
      DIR* d = opendir(full.c_str());
      if (!d) Out.println("  can't open " + path);
      else {
        Out.println("  " + path);
        struct dirent* e;
        while ((e = readdir(d)) != nullptr) {
          struct stat st;
          bool dir = e->d_type == DT_DIR;
          if (!dir && stat((full + "/" + e->d_name).c_str(), &st) == 0)
            Out.printf("    %-32s %s\n", e->d_name, fmtBytes(st.st_size).c_str());
          else Out.printf("    %s/\n", e->d_name);
        }
        closedir(d);
      }
    }
    if (!was && screen != SCREEN_SD) sdUnmount();
  } else if (lower.startsWith("city ")) {
    String c = in.substring(5); c.trim();
    strlcpy(wxQuery, c.c_str(), sizeof(wxQuery));
    Out.println("Weather: looking up '" + c + "'...");
    if (isZip(c.c_str())) prefs.putString("wxZip", c);
  } else if (lower.startsWith("loc ")) {
    float la, lo;
    if (sscanf(in.c_str() + 4, "%f%*[ ,]%f", &la, &lo) == 2 && fabsf(la) <= 90 && fabsf(lo) <= 180) {
      wxLat = la; wxLon = lo; wxHaveLoc = true;
      wxPlace = String(la, 3) + ", " + String(lo, 3);
      prefs.putFloat("wxLat", la); prefs.putFloat("wxLon", lo); prefs.putString("wxPlace", wxPlace);
      wxSoon = true;
      Out.println("Weather place set to " + wxPlace);
    } else Out.println("loc <latitude>,<longitude>   e.g. loc 45.52,-122.68");
  } else if (lower == "wx") {
    WxState w;
    xSemaphoreTake(nestLock, portMAX_DELAY); w = wx; xSemaphoreGive(nestLock);
    if (!wxHaveLoc) Out.println("Weather: no place set - send: city <name>");
    else {
      Out.printf("Weather: %s (%.3f, %.3f)\n", wxPlace.c_str(), wxLat, wxLon);
      if (w.valid) Out.printf("  now %.1f C, %s, read %lus ago\n", w.curC, wxText(w.curCode), (millis() - w.updatedMs) / 1000);
      if (w.error[0]) Out.println(String("  last error: ") + w.error);
      wxSoon = true;
      Out.println("  refreshing...");
    }
  } else if (lower == "snd") {
    Out.printf("Alarm sound: %s, music %s, player %s, %lu chunks, %lu bytes to I2S, audio %s\n",
                  alarmFile.length() ? alarmFile.c_str() : "beep", alarmMusic ? "on" : "off",
                  player ? (player->isRunning() ? "running" : "idle") : "not created",
                  (unsigned long)musicChunks, (unsigned long)musicBytes, audioOk ? "ok" : "NOT running");
  } else if (lower == "power") {
    unsigned long up = millis(), asleep = statAsleepMs + (sleeping ? millis() - statSleepStart : 0);
    Out.printf("Since power-up: %lu min, asleep %lu min (%d%%)\n", up / 60000, asleep / 60000,
               up ? (int)(asleep * 100 / up) : 0);
    Out.printf("Wakes: touch %lu, tap/move %lu, alarm %lu, USB %lu\n", (unsigned long)statWakeTouch,
               (unsigned long)statWakeMotion, (unsigned long)statWakeAlarm, (unsigned long)statWakeUsb);
    if (batPresent) Out.printf("Battery %d%%, %.2f V, %s\n", batPct, batMv / 1000.0,
                               batCharging ? "charging" : usbIn ? "on USB" : "on battery");
  } else if (lower == "status") {
    struct tm t;
    if (nowLocal(t)) { char b[40]; strftime(b, sizeof(b), "%a %Y-%m-%d %H:%M:%S %Z", &t); Out.println(String("Time: ") + b); }
    else Out.println("Time: not set yet");
    Out.println("Wi-Fi: " + (wifiSsid.length() ? wifiSsid : String("not set")) +
                   (WiFi.status() == WL_CONNECTED ? " (connected, " + String(WiFi.RSSI()) + " dBm)" : " (not connected)"));
    Out.println("Last NTP sync: " + fmtTime(lastSyncOk));
    Out.println(String("RTC: ") + (rtcFound ? (rtcValidAtBoot ? "found, valid at boot" : "found, had lost its time") : "not found"));
    if (batPresent) Out.printf("Battery: %d%%, %.2f V, %s\n", batPct, batMv / 1000.0,
                                  batCharging ? "charging" : (usbIn ? "on USB, not charging" : "on battery"));
    else Out.println(String("Battery: none") + (usbIn ? " (on USB)" : ""));
    Out.printf("Charge: %d mA to %.2f V\n", chargeReadMa(), chargeReadMv() / 1000.0);
    Out.println(String("Nest: ") + (nestConfigured() ? "set up (nest status for details)" : "not set up"));
  } else if (lower.startsWith("chargev")) {
    String a = lower.substring(7); a.trim();
    if (a.length()) {
      float v = a.toFloat();
      if (v > 100) v /= 1000.0f;           // accept 4200 as well as 4.2
      if (fabsf(v - 4.1f) < 0.02f) chargeMv = 4100;
      else if (fabsf(v - 4.2f) < 0.02f) chargeMv = 4200;
      else { Out.println("chargev: 4.1 or 4.2 (V)"); return; }
      prefs.putInt("chgMv", chargeMv);
      chargeApply();
    }
    Out.printf("Charge to %.2f V (chip reports %.2f V)\n", chargeMv / 1000.0, chargeReadMv() / 1000.0);
  } else if (lower == "charge" || lower.startsWith("charge ")) {
    String a = lower.substring(6); a.trim();
    if (a.length()) {
      int ma = a.toInt();
      if (ma < 200 || ma > 1000) { Out.println("charge: 200-1000 mA (100 mA steps)"); return; }
      chargeMa = (ma + 50) / 100 * 100;   // nearest 100 mA
      prefs.putInt("chgMa", chargeMa);
      chargeApply();
    }
    Out.printf("Charge current %d mA (chip reports %d mA), charge to %.2f V\n", chargeMa, chargeReadMa(),
               chargeReadMv() / 1000.0);
  } else if (lower == "face" || lower.startsWith("face ")) {
    String a = lower.substring(4);
    a.trim();
    int f = -1;
    for (int i = 0; i < N_FACES; i++) { String n = FACE_NAMES[i]; n.toLowerCase(); if (a == n) f = i; }
    if (a.length() == 0) f = (faceStyle + 1) % N_FACES;
    if (f < 0) { Out.println("face jump | analog | digital"); return; }
    faceStyle = f;
    prefs.putInt("face", faceStyle);
    Out.printf("Clock face: %s\n", FACE_NAMES[faceStyle]);
    updateClock(true);
  } else if (lower.startsWith("bl")) {
    blPct = constrain((lower.substring(2).toInt() + 5) / 10 * 10, 10, 100);
    prefs.putInt("blPct", blPct);
    applyBacklight(false);
    Out.printf("Battery brightness %d%% (USB: full)\n", blPct);
  } else if (lower.startsWith("alarm")) {
    String a = lower.substring(5); a.trim();
    int h, m;
    if (a == "on" || a == "off") alarmOn = (a == "on");
    else if (sscanf(a.c_str(), "%d:%d", &h, &m) == 2 && h >= 0 && h < 24 && m >= 0 && m < 60) { alarmH = h; alarmM = m; alarmOn = true; }
    else if (a == "test") { ringClockAlarm(true); return; }
    else { Out.println("alarm HH:MM | alarm on | alarm off | alarm test"); return; }
    prefs.putInt("alH", alarmH); prefs.putInt("alM", alarmM); prefs.putBool("alOn", alarmOn);
    Out.printf("Alarm %02d:%02d %s\n", alarmH, alarmM, alarmOn ? "ON" : "OFF");
    updateClock(true);
  } else if (lower == "chime" || lower.startsWith("chime ")) {
    String a = lower.substring(5); a.trim();
    int f, t, v;
    if (a == "on" || a == "off") { chimeOn = (a == "on"); prefs.putBool("chOn", chimeOn); }
    else if (a == "test") { endChime(); startChime(); }
    else if (a.startsWith("vol") && sscanf(a.c_str() + 3, "%d", &v) == 1) {
      chimeVol = constrain(v, 0, 100); prefs.putInt("chVol", chimeVol);
    }
    else if (a == "quiet on" || a == "quiet off") { quietOn = (a == "quiet on"); prefs.putBool("chQt", quietOn); }
    else if (sscanf(a.c_str(), "quiet %d-%d", &f, &t) == 2 && f >= 0 && f < 24 && t >= 0 && t < 24) {
      quietFrom = f; quietTo = t; quietOn = true; saveSound(); prefs.putBool("chQt", true);
    }
    else if (a.length()) { Out.println("chime on|off|test | chime vol <0-100> | chime quiet on|off|HH-HH"); return; }
    Out.printf("Chime %s, volume %d%%, sound %s, quiet hours %s %02d-%02d%s\n", chimeOn ? "ON" : "OFF", chimeVol,
               chimeFile.length() ? chimeFile.c_str() : "bell", quietOn ? "ON" : "OFF", quietFrom, quietTo,
               inQuietHours() ? " (quiet now)" : "");
    if (screen == SCREEN_SOUND) renderSoundPage();
    updateClock(true);
  } else if (lower == "demo") {
    struct tm t;
    if (!nowLocal(t)) { Out.println("No time yet"); return; }
    demoOffset = (59 - t.tm_min) * 60L + (55 - t.tm_sec);
    demoActive = true;
    demoStart = millis();
    updateClock(true);
  } else {
    Out.println("ssid | pass | wifi | bt | sync | status | power | charge | chargev | face | bl <10-100> | alarm HH:MM|on|off|test | snd | chime | demo | city | loc | wx | sd | orient | nest ...");
  }
}

// ============================================================================
// SETUP / LOOP
// ----------------------------------------------------------------------------
// Arduino entry points.
// ============================================================================

// loop() runs with a 16 KB stack instead of the default 8 KB. The smooth-font
// renderer (LovyanGFX VLW) unpacks each glyph on the stack: one digit of the
// big sideways DIGITAL clock is ~7 KB, which overflowed the 8 KB stack and
// rebooted the board when it was turned to the clock-only position.
// (Kept down here on purpose: the macro defines a function, and the Arduino
// IDE inserts its automatic prototypes before the first function in the
// file - near the top, that was before the Btn type existed.)
SET_LOOP_TASK_STACK_SIZE(16 * 1024);

void setup() {
  Serial.begin(115200);
#if ARDUINO_USB_MODE == 1 && ARDUINO_USB_CDC_ON_BOOT
  // Never wait on USB: with the cable in but no Serial Monitor reading,
  // writes would otherwise block (the clock looked frozen)
  Serial.setTxTimeoutMs(0);
#endif
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 1500);

  nestLock = xSemaphoreCreateMutex();
  prefs.begin("clocknest", false);
  wifiSsid    = prefs.getString("ssid", "");
  wifiPass    = prefs.getString("pass", "");
  faceStyle   = constrain(prefs.getInt("face", FACE_JUMP), 0, N_FACES - 1);
  if (prefs.isKey("blPct")) blPct = prefs.getInt("blPct", 80);
  else if (prefs.isKey("bl")) blPct = (prefs.getInt("bl", 204) * 100 / 255 + 5) / 10 * 10;   // old setting
  blPct = constrain(blPct, 10, 100);
  sleepIdx    = constrain(prefs.getInt("sleepI", 3), 0, N_SLEEP - 1);
  chargeMa    = constrain(prefs.getInt("chgMa", 400), 200, 1000);
  chargeMv    = prefs.getInt("chgMv", 4200) >= 4200 ? 4200 : 4100;
  keepWifiAsleep = prefs.getBool("slpWifi", true);
  keepBtAsleep   = prefs.getBool("slpBt", false);
  wakeOnMotion   = prefs.getBool("wakeMov", true);
  alarmH      = prefs.getInt("alH", 7);
  alarmM      = prefs.getInt("alM", 0);
  alarmOn     = prefs.getBool("alOn", false);
  alarmFile   = prefs.getString("alSnd", "");
  alarmVol    = constrain(prefs.getInt("alVol", 80), 0, 100);
  chimeOn     = prefs.getBool("chOn", true);
  chimeVol    = constrain(prefs.getInt("chVol", 50), 0, 100);
  chimeFile   = prefs.getString("chSnd", "");
  quietOn     = prefs.getBool("chQt", true);
  quietFrom   = constrain(prefs.getInt("chQF", 22), 0, 23);
  quietTo     = constrain(prefs.getInt("chQT", 7), 0, 23);
  nestProject = prefs.getString("nProj", "");
  nestClient  = prefs.getString("nCli", "");
  nestSecret  = prefs.getString("nSec", "");
  nestRefresh = prefs.getString("nRef", "");
  wxHaveLoc   = prefs.isKey("wxLat");
  wxLat       = prefs.getFloat("wxLat", 0);
  wxLon       = prefs.getFloat("wxLon", 0);
  wxPlace     = prefs.getString("wxPlace", "");
  wifiOn      = prefs.getBool("wifiOn", true);
  btOn        = prefs.getBool("btOn", false);
  btPin       = prefs.getUInt("btPin", 0);
  orientAuto  = prefs.getBool("orAuto", true);
  orientFlip  = prefs.getBool("orFlip2", false);
  orientSwap  = prefs.getBool("orSwap2", false);
  if (btPin < 100000) { btPin = 100000 + (esp_random() % 900000); prefs.putUInt("btPin", btPin); }

  Wire.begin(PIN_SDA, PIN_SCL, 400000);
  lcdHardReset();
  lcd.init();
  lcd.setRotation(0);                     // portrait 320 x 480
  lcd.setBrightness(backlight);            // full until the battery state is known
  frame.setPsram(true);
  frame.setColorDepth(16);
  if (!frame.createSprite(W, H)) Serial.println("PSRAM frame FAILED - set PSRAM: OPI PSRAM");

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
  }

  audioInit();
  cardFs.setMount(SD_BASE);
  if (audioOk) setAlarmVolume(alarmVol);
#if HAVE_USB_DRIVE
  usbDriveSetup();
#endif
  batteryInit();
  batteryRead();
  imuOk = imuInit();
  applyBacklight(false);
  // NTP: at power-up, then once a day (SNTP repeats on its own)
  sntp_set_time_sync_notification_cb(onSntpSync);
  sntp_set_sync_interval(24UL * 3600UL * 1000UL);
  configTzTime(TZ_INFO, NTP1, NTP2);
  if (wifiOn) wifiStart(); else WiFi.mode(WIFI_OFF);
  if (btOn) bleStart();
  xTaskCreatePinnedToCore(nestTask, "nest", 12288, nullptr, 1, nullptr, 0);
  struct tm tb;                          // no chime just for powering up at hh:00
  if (nowLocal(tb)) lastChimeKey = tb.tm_yday * 24L + tb.tm_hour;
  updateClock(true);
}

// loop(): Main loop (core 1, about every 15 ms): commands, NTP result, alarm, music,
//   sensors, battery, sleep, touch dispatch for whichever screen is shown, and
//   redraws. Anything slow (network) is in nestTask instead.
void loop() {
  // ---- 1. Serial Monitor: collect characters until Enter, then run the line
  static String buf;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { if (buf.length()) handleCommand(buf); buf = ""; }
    else if (buf.length() < 600) buf += c;      // (refresh tokens are long)
  }

  // ---- 2. internet time arrived (flag set by the SNTP callback)
  if (sntpDone) {                         // NTP answered: keep the RTC in step
    sntpDone = false;
    lastSyncOk = time(nullptr);
    if (rtcFound) rtcWrite(lastSyncOk);
    Serial.println("NTP synced: " + fmtTime(lastSyncOk) + " - RTC updated");
    if (clockSyncAsked) {
      clockSyncAsked = false;
      if (screen == SCREEN_SETTINGS) {
        struct tm t;
        if (nowLocal(t)) { editH = t.tm_hour; editM = t.tm_min; }
        savedMsg = "Time synced"; savedMsgColor = C_OK; savedMsgUntil = millis() + 2000;
        renderSettings();
      }
    }
  }

  // ---- 3. "demo" command: back to the real time after 8 s
  if (demoActive && millis() - demoStart >= 8000) {
    demoActive = false; demoOffset = 0; shownHour = -1;
    updateClock(true);
  }

  // ---- 4. alarm: ring once when the clock reaches the alarm minute
  //         (lastAlarmKey = day+minute, so it can't ring twice in one minute)
  struct tm now;
  if (alarmOn && !demoActive && nowLocal(now) && now.tm_hour == alarmH && now.tm_min == alarmM) {
    long key = now.tm_yday * 1440L + alarmH * 60 + alarmM;
    if (key != lastAlarmKey) { lastAlarmKey = key; ringClockAlarm(false); }
  }
  if (alarmRinging && millis() - alarmStartMs >= alarmLimitMs) stopAlarm();
  musicService();                        // keep the alarm (or chime) music fed
  chimeService();                        // hourly chime, Sound page TEST / volume preview
  // volume changed on the Clock page: three beeps 0.4 s after the last tap
  if (volChangedAt && millis() - volChangedAt > 400 && !alarmRinging && audioOk) {
    volChangedAt = 0;
    endChime();                          // (the codec goes back to the alarm volume)
    if (alarmVol > 0) { ampOn(true); previewBeeps = 3; previewAmpUntil = millis() + 900; }
  }
  if (previewAmpUntil && millis() > previewAmpUntil && previewBeeps == 0) {
    previewAmpUntil = 0;
    if (!alarmRinging && !chimeActive) ampOn(false);
  }

  // ---- 5. Nest: send an adjusted target 1.5 s after the last tap
  //         (the network task picks up cmdType and does the HTTPS call)
  if (pendingSet && millis() - pendLastTap > 1500 && cmdType == 0) {
    cmdHeatC = pendHeatC; cmdCoolC = pendCoolC;
    cmdType = 1;
    pendingSet = false;
    sentShow = true;
    sentVersion = nestVersion;
    nestNote("sending...");
  }

  // ---- 6. commands from a paired phone (secrets are not echoed to Serial)
  if (bleHasCmd) {
    String c = bleInbox;
    bleHasCmd = false;
    c.trim();
    if (c.length()) { Serial.println("BLE> " + (c.startsWith("pass ") || c.startsWith("nest secret") || c.startsWith("nest token") ? c.substring(0, 5) + "..." : c)); handleCommand(c); }
  }
  // ---- 7. background jobs: Wi-Fi (re)connected, scan results, motion sensor, battery
  static bool wasConnected = false;      // Wi-Fi just came up: fetch weather + Nest now
  bool isConnected = WiFi.status() == WL_CONNECTED;
  if (isConnected && !wasConnected) { wxSoon = true; nestPollSoon = true; Serial.println("Wi-Fi up: refreshing weather and Nest"); }
  wasConnected = isConnected;
  bool wasScanning = wifiScanning;
  wifiScanPoll();
  if (wasScanning && !wifiScanning && screen == SCREEN_WIFI) renderWifiPage();

  orientService();

  // battery: read every 5 s (the face redraws every second anyway)
  static unsigned long lastBat = 0;
  if (millis() - lastBat >= 5000) { lastBat = millis(); batteryRead(); }

  // ---- 8. touch: 'tnow' = finger on the glass, 'down' = it just landed
  static bool wasTouched = false;
  static unsigned long pressStart = 0, lastRepeat = 0;
  int tx = 0, ty = 0;
  bool tnow = readTouch(tx, ty);
  bool down = tnow && !wasTouched;
  wasTouched = tnow;
  if (tnow) lastTouchMs = millis();
  bool redraw = false;

  // ---- 9. sleep on battery: fade out after the chosen idle time; while dark,
  //         only a touch, the alarm or USB power does anything
  //         (and that waking touch is "swallowed" so it presses nothing)
  static bool wasOnBattery = false, swallowTouch = false;
  bool onBattery = batPresent && !usbIn;
  if (onBattery != wasOnBattery) {         // USB plugged in / unplugged
    wasOnBattery = onBattery;
    lastTouchMs = millis();                // restart the sleep timer
    if (!sleeping) applyBacklight(true);   // full on USB, chosen level on battery
  }
  if (blPreviewUntil && millis() > blPreviewUntil && !sleeping) {
    blPreviewUntil = 0;
    lcd.setBrightness(backlight);          // back to the level for this power source
  }
  if (sleeping) {
    // tap / move / shake: sample the accelerometer every 5 ms while dark.
    //   tap or shake = a sudden jump between two samples (> 0.15 g, summed
    //   over x, y, z); move / pick up = drifted from the position it fell
    //   asleep in (> 0.2 g, about a 12 deg tilt).
    bool moved = false;
    if (wakeOnMotion && imuOk) {
      for (int i = 0; i < 6 && !moved; i++) {
        float px = accX, py = accY, pz = accZ;
        if (imuRead()) {
          float jump  = fabsf(accX - px) + fabsf(accY - py) + fabsf(accZ - pz);
          float drift = fabsf(accX - motRefX) + fabsf(accY - motRefY) + fabsf(accZ - motRefZ);
          if (jump > 0.15f || drift > 0.20f) moved = true;
        }
        if (!moved) delay(5);
      }
    } else delay(30);
    if (down || alarmRinging || !onBattery || moved) {
      if (alarmRinging) statWakeAlarm++;
      else if (!onBattery) statWakeUsb++;
      else if (down) statWakeTouch++;
      else { statWakeMotion++; Serial.println("Wake: motion"); }
      wakeUp();
      swallowTouch = tnow;
    }
    else return;                         // dark: nothing to draw
  }
  if (swallowTouch) {                    // the touch that woke it does nothing else
    if (!tnow) swallowTouch = false;
    tnow = down = false;
  }
  // alarm ringing: the backlight flashes too, in step with the bell
  //   (bright while the bell shows, almost dark in between)
  static int ringBl = -1;                // level last set by the flashing, -1 = not flashing
  if (alarmRinging) {
    int lvl = ringBlink() ? 255 : 8;
    if (lvl != ringBl) { lcd.setBrightness(lvl); ringBl = lvl; }
  } else if (ringBl >= 0) {
    ringBl = -1;
    lcd.setBrightness(backlight);        // back to the normal level
  }

  if (onBattery && SLEEP_OPTS[sleepIdx] > 0 && !alarmRinging &&
      millis() - lastTouchMs > (unsigned long)SLEEP_OPTS[sleepIdx] * 1000UL) { goSleep(); return; }

  // ---- 10. hand the touch to whatever is on screen. Pattern on every page:
  //          down -> hitTest() -> pressXxx(); finger held on an arrow ->
  //          auto-repeat after 0.45 s; finger lifted -> un-highlight the button.
  //          Any touch while ringing just stops the alarm.
  if (down && alarmRinging) {
    stopAlarm();
    pressedBtn = B_NONE;
    redraw = true;
  } else if (screen == SCREEN_FACE && isSideways()) {
    // sideways: nothing to tap
  } else if (screen == SCREEN_FACE) {
    // pull-down from the top edge: open settings once the finger has moved 60 px
    if (swipeArmed && !down) {
      if (!tnow) swipeArmed = false;
      else if (ty - swipeY0 > 60) { swipeArmed = false; slideSettingsOpen(); }
    }
    if (screen != SCREEN_FACE) {
      // settings just opened
    } else if (down) {
      if (ty < 45) { swipeArmed = true; swipeY0 = ty; }                        // top edge: maybe a pull-down
      else if (tx < PILL_X + PILL_W + 8 && ty >= PILL_Y - 6 && ty < PILL_Y + PILL_H + 8) {
        panelMode = (panelMode + 1) % 2;     // Nest <-> weather
        nestShownAt = millis();
        pressedBtn = -1;
        redraw = true;
        WxState w;
        xSemaphoreTake(nestLock, portMAX_DELAY); w = wx; xSemaphoreGive(nestLock);
        if (panelMode == 1 && (!w.valid || millis() - w.updatedMs > 15UL * 60UL * 1000UL)) wxSoon = true;
      } else if (panelMode == 0) {
        pressedBtn = hitTest(NBTNS, NNB, tx, ty);
        pressStart = lastRepeat = millis();
        if (pressedBtn != B_NONE) { pressNest(pressedBtn); redraw = true; }
      }
    } else if (tnow && isArrow(pressedBtn) && millis() - pressStart > 450 && millis() - lastRepeat > 160) {
      lastRepeat = millis();
      pressNest(pressedBtn);
      redraw = true;
    } else if (!tnow && pressedBtn != B_NONE) { pressedBtn = B_NONE; redraw = true; }
  } else if (screen == SCREEN_SD) {
    if (down) {
      pressedBtn = sdConfirm ? hitTest(SDCBTNS, NSDC, tx, ty) : hitTest(SDBTNS, NSDB, tx, ty);
      if (pressedBtn != B_NONE) pressSd(pressedBtn);
      else if (!sdConfirm && ty >= SD_ROW_Y && ty < SD_ROW_Y + SD_ROWS * SD_ROW_H) { sdTapRow(ty); if (screen == SCREEN_SD) renderSd(); }
    } else if (!tnow && pressedBtn != B_NONE) {
      pressedBtn = B_NONE;
      renderSd();
    }
    if (screen == SCREEN_SD && !sdConfirm && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_WIFI) {
    if (down) {
      pressedBtn = hitTest(WIFIBTNS, NWIFIB, tx, ty);
      if (pressedBtn != B_NONE) pressWifi(pressedBtn);
      else if (ty >= WL_ROW_Y && ty < WL_ROW_Y + WL_ROWS * WL_ROW_H) wifiTapRow(ty);
    } else if (!tnow && pressedBtn != B_NONE) { pressedBtn = B_NONE; renderWifiPage(); }
    if (screen == SCREEN_WIFI && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_KBD) {
    if (down) {
      pressedBtn = hitTest(HDRBTNS, 1, tx, ty);
      if (pressedBtn == B_BACK) { pressedBtn = B_NONE; screen = SCREEN_WIFI; renderWifiPage(); }
      else if (tx >= 258 && ty >= 84 && ty < 134) { kbShow = !kbShow; renderKbd(); }
      else {
        for (int i = 0; i < nKeys; i++)
          if (tx >= keys[i].x - 1 && tx < keys[i].x + keys[i].w + 2 && ty >= keys[i].y - 2 && ty < keys[i].y + keys[i].h + 3) {
            pressedKey = i; pressKey(i); break;
          }
      }
    } else if (!tnow && pressedKey >= 0) { pressedKey = -1; if (screen == SCREEN_KBD) renderKbd(); }
    if (screen == SCREEN_KBD && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_USB) {
    if (down) {
      pressedBtn = hitTest(USBBTNS, 1, tx, ty);
      if (pressedBtn != B_NONE) pressUsb(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) { pressedBtn = B_NONE; renderUsbPage(); }
  } else if (screen == SCREEN_DISP) {
    if (down) {
      pressedBtn = hitTest(DISPBTNS, NDISPB, tx, ty);
      pressStart = lastRepeat = millis();
      if (pressedBtn != B_NONE) pressDisp(pressedBtn);
    } else if (tnow && (pressedBtn == B_D_BLDN || pressedBtn == B_D_BLUP) && millis() - pressStart > 450 && millis() - lastRepeat > 120) {
      lastRepeat = millis();
      pressDisp(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) {
      if (pressedBtn == B_D_BLDN || pressedBtn == B_D_BLUP) prefs.putInt("blPct", blPct);
      pressedBtn = B_NONE;
      renderDispPage();
    }
    if (screen == SCREEN_DISP && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_BT) {
    if (down) {
      pressedBtn = hitTest(BTBTNS, NBTB, tx, ty);
      if (pressedBtn != B_NONE) pressBt(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) { pressedBtn = B_NONE; renderBtPage(); }
    if (screen == SCREEN_BT && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_WX) {
    if (down) {
      pressedBtn = hitTest(WXBTNS, NWXB, tx, ty);
      if (pressedBtn != B_NONE) pressWx(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) {
      pressedBtn = B_NONE;
      renderWxPage();
    }
    static int wxSeen = -1;                // lookup finished: show the result
    if (screen == SCREEN_WX && nestVersion != wxSeen) { wxSeen = nestVersion; renderWxPage(); }
    if (screen == SCREEN_WX && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_SOUND) {
    if (down) {
      pressedBtn = hitTest(SNDBTNS, NSNDB, tx, ty);
      pressStart = lastRepeat = millis();
      if (pressedBtn != B_NONE) pressSound(pressedBtn);
    } else if (tnow && isArrow(pressedBtn) && millis() - pressStart > 450 && millis() - lastRepeat > 110) {
      lastRepeat = millis();
      pressSound(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) {
      saveSound();
      pressedBtn = B_NONE;
      if (screen == SCREEN_SOUND) renderSoundPage();
    }
    if (screen == SCREEN_SOUND && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else if (screen == SCREEN_MENU) {
    if (down) {
      pressedBtn = hitTest(MBTNS, NMB, tx, ty);
      if (pressedBtn != B_NONE) pressMenu(pressedBtn);
      else { swipeArmed = true; swipeY0 = ty; }          // not on a tile: maybe a swipe up
    } else if (swipeArmed) {
      if (!tnow) swipeArmed = false;
      else if (swipeY0 - ty > 60) { swipeArmed = false; slideSettingsClosed(); }
    } else if (!tnow && pressedBtn != B_NONE) {
      pressedBtn = B_NONE;
      renderMenu();
    }
    if (screen == SCREEN_MENU && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  } else {
    if (down) {
      pressedBtn = hitTest(SBTNS, NSB, tx, ty);
      pressStart = lastRepeat = millis();
      if (pressedBtn != B_NONE) pressSettings(pressedBtn);
    } else if (tnow && isArrow(pressedBtn) && millis() - pressStart > 450 && millis() - lastRepeat > 110) {
      lastRepeat = millis();
      pressSettings(pressedBtn);
    } else if (!tnow && pressedBtn != B_NONE) {
      pressedBtn = B_NONE;
      if (screen == SCREEN_SETTINGS) renderSettings();
    }
    if (screen == SCREEN_SETTINGS && millis() - lastTouchMs > SETTINGS_IDLE_MS) slideSettingsClosed();
  }

  // ---- 11. redraw: the face once a second (or now if something changed:
  //          new Nest/weather data, a note appearing, the alarm flashing);
  //          settings pages only when needed and once a second for live status
  if (screen == SCREEN_FACE) {
    static int seenVersion = -1;
    if (nestVersion != seenVersion) { seenVersion = nestVersion; redraw = true; }
    static bool noteShown = false;
    bool noteNow = millis() < nestMsgUntil;
    if (noteShown != noteNow) redraw = true;
    noteShown = noteNow;
    static bool wasRinging = false;
    // the Nest panel returns to weather after 15 s without a touch
    if (panelMode == 0 && !pendingSet && cmdType == 0 &&
        millis() - nestShownAt > 15000 && millis() - lastTouchMs > 15000) {
      panelMode = 1;
      redraw = true;
    }
    static unsigned long lastFlash = 0;
    if ((alarmRinging || wasRinging) && millis() - lastFlash >= 200) { lastFlash = millis(); redraw = true; }
    wasRinging = alarmRinging;
    updateClock(redraw);
  } else {
    static unsigned long lastRedraw = 0;
    static unsigned long lastStatus = 0;  // status pages: refresh each second
    if ((screen == SCREEN_WIFI || screen == SCREEN_BT || screen == SCREEN_MENU || screen == SCREEN_DISP || screen == SCREEN_USB ||
         screen == SCREEN_SOUND) &&
        millis() - lastStatus > (screen == SCREEN_USB ? 300UL : 1000UL)) {
      lastStatus = millis();
      redraw = true;
    }
    if ((alarmRinging || redraw || millis() < savedMsgUntil + 300) && millis() - lastRedraw > 200) {
      lastRedraw = millis();
      renderCurrent();
    }
  }
  // ~15 ms per pass: touch feels instant, and the network task gets CPU time
  delay(15);
}
