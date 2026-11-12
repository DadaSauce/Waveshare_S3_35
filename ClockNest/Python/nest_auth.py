#!/usr/bin/env python3
"""
Nest sign-in helper for the WS35_ClockNest sketch (Waveshare ESP32-S3 3.5").

What it does:
  1. asks for your Device Access project ID, OAuth client ID and secret
  2. opens Google's sign-in page in your browser - you allow access to
     the thermostat
  3. trades the code Google gives you for a long-lived "refresh token"
  4. checks that it can see your thermostat
  5. sends everything to the board over USB; the board saves it in its
     flash memory

Nothing is written to disk on the Mac, and nothing goes into the sketch.

Setup (once):  pip3 install pyserial
Run:           python3 nest_auth.py            (finds the board by itself)
               python3 nest_auth.py /dev/cu.usbmodem1101
               python3 nest_auth.py --no-board (only show the token)
Close the Arduino Serial Monitor first - only one program can use the port.
"""

import getpass
import glob
import json
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
import webbrowser

REDIRECT = "https://www.google.com"
SCOPE = "https://www.googleapis.com/auth/sdm.service"
TOKEN_URL = "https://www.googleapis.com/oauth2/v4/token"
SDM = "https://smartdevicemanagement.googleapis.com/v1/enterprises/"


def post_form(url, fields):
    data = urllib.parse.urlencode(fields).encode()
    req = urllib.request.Request(url, data=data, method="POST")
    try:
        with urllib.request.urlopen(req, timeout=20) as r:
            return json.load(r)
    except urllib.error.HTTPError as e:
        sys.exit(f"Google said {e.code}: {e.read().decode(errors='replace')}")


def get_json(url, token):
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}"})
    try:
        with urllib.request.urlopen(req, timeout=20) as r:
            return json.load(r)
    except urllib.error.HTTPError as e:
        sys.exit(f"Reading devices failed ({e.code}): {e.read().decode(errors='replace')}")


def ask(prompt, secret=False):
    while True:
        v = (getpass.getpass(prompt) if secret else input(prompt)).strip()
        if v:
            return v


def code_from(text):
    """Accept either the whole address bar URL or just the code."""
    text = text.strip()
    if "code=" in text:
        q = urllib.parse.urlparse(text).query
        return urllib.parse.parse_qs(q).get("code", [""])[0]
    return text


def send_to_board(port, lines):
    try:
        import serial
    except ImportError:
        sys.exit("Missing library - run:  pip3 install pyserial")
    if not port:
        ports = sorted(glob.glob("/dev/cu.usbmodem*"))
        if not ports:
            sys.exit("Board not found (no /dev/cu.usbmodem*). Plug it in, or give the port.")
        port = ports[0]
    ser = serial.Serial()
    ser.port, ser.baudrate, ser.timeout = port, 115200, 0.2
    ser.dtr = False                      # don't reset the board on open
    ser.rts = False
    ser.open()
    time.sleep(0.5)
    ser.reset_input_buffer()
    print(f"\nSending to the board on {port}:")
    for line in lines:
        ser.write((line + "\n").encode())
        time.sleep(0.4)
    end = time.time() + 6                # show what the board answers
    while time.time() < end:
        out = ser.readline().decode(errors="replace").strip()
        if out:
            print("  board:", out)
    ser.close()


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    no_board = "--no-board" in sys.argv
    port = args[0] if args else None

    print("Nest sign-in for the clock\n")
    project = ask("Device Access project ID (a UUID): ")
    client = ask("OAuth client ID (...apps.googleusercontent.com): ")
    secret = ask("OAuth client secret (hidden): ", secret=True)

    url = (f"https://nestservices.google.com/partnerconnections/{project}/auth?"
           + urllib.parse.urlencode({
               "redirect_uri": REDIRECT, "access_type": "offline", "prompt": "consent",
               "client_id": client, "response_type": "code", "scope": SCOPE}))
    print("\nOpening Google in your browser. Sign in with the account that owns the")
    print("Nest, turn ON access to the thermostat, then Allow.")
    print("You'll land on google.com - copy the WHOLE address from the address bar.\n")
    print("(If the browser doesn't open, paste this link into it:)\n" + url + "\n")
    webbrowser.open(url)
    code = code_from(ask("Paste the address (or just the code) here: "))

    tok = post_form(TOKEN_URL, {
        "client_id": client, "client_secret": secret, "code": code,
        "grant_type": "authorization_code", "redirect_uri": REDIRECT})
    refresh = tok.get("refresh_token")
    access = tok.get("access_token")
    if not refresh:
        sys.exit("No refresh token in Google's reply - run again (the link asks for a fresh consent).")

    # this first read also finishes the account link on Google's side
    devices = get_json(SDM + project + "/devices", access).get("devices", [])
    stats = [d for d in devices if d.get("type") == "sdm.devices.types.THERMOSTAT"]
    if not stats:
        sys.exit("Signed in, but no thermostat is shared with this project. Run again and "
                 "make sure the thermostat is switched on in the permissions page.")
    for d in stats:
        tr = d.get("traits", {})
        room = (d.get("parentRelations") or [{}])[0].get("displayName", "")
        c = tr.get("sdm.devices.traits.Temperature", {}).get("ambientTemperatureCelsius")
        mode = tr.get("sdm.devices.traits.ThermostatMode", {}).get("mode")
        print(f"Found thermostat {room or ''}: {c} C, mode {mode}")
    if len(stats) > 1:
        print("(The clock shows the first one.)")

    if no_board:
        print("\nRefresh token (keep it private):\n" + refresh)
        print("\nOn the board's Serial Monitor send:")
        print(f"  nest project {project}\n  nest client {client}\n"
              "  nest secret <your secret>\n  nest token <the token above>")
        return

    send_to_board(port, [
        f"nest project {project}",
        f"nest client {client}",
        f"nest secret {secret}",
        f"nest token {refresh}",
        "nest status",
    ])
    print("\nDone. The bottom half of the clock should fill in within a few seconds.")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")
