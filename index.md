---
title: ClockNest Home Hub
---

# ClockNest Home Hub

A personal, hobby project: desk clocks and e-ink dashboards (a Waveshare
ESP32-S3 3.5" display and two Kindle Touch e-readers) that show the time,
the weather and my own Google Nest thermostat.

## What it does with Google

The devices sign in with Google once, through the Google Smart Device
Management API, to **read** the thermostat's temperature, humidity and
heating/cooling status and to **change its set point** from the device's
touch screen. Nothing else in the Google account is used.

- Built for and used only by its owner, on devices in the owner's home.
- No server: each device talks to Google directly.
- No data is collected, sold or shared with anyone.

See the [Privacy Policy](PRIVACY.html).

Source code: [github.com/DadaSauce/Waveshare_S3_35](https://github.com/DadaSauce/Waveshare_S3_35)
