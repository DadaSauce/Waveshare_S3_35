# Waveshare ESP32-S3-Touch-LCD-3.5 projects

| Folder | What it is |
| --- | --- |
| `ClockNest/WS35_ClockNest` | Home hub: 24-hour clock with three faces (jump-hour, analog, digital), weather, Google Nest, alarm with SD music, hourly chime with quiet hours, settings menu, sideways layouts |
| `ClockNest/WS35_ClockNest/clock_fonts.h` | Anti-aliased DejaVu fonts for the digital face (generated; don't edit by hand) |
| `ClockNest/Python/nest_auth.py` | Mac helper for the one-time Google (Nest) sign-in |
| `ClockNest/ClockNest_Setup_Guide.pdf` | Full setup guide |
| `ClockNest/ESP32-audioI2S-master.zip` | Audio library (Waveshare's copy, v3.3.0) |
| `WS35_JumpHour` | Earlier landscape jump-hour clock |
| `WS35_LGFX_Test`, `WS35_Test` | Display bring-up tests |

No secrets are stored in this repo: Wi-Fi and Nest credentials live in the board's flash, and the Google `client_secret_*.json` is git-ignored.
