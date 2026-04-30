# Setup Guide — One Camera, Gmail

## What you need before starting

- Python 3.10 or newer ([python.org](https://www.python.org/downloads/))
- Optris PIX Connect software installed on this PC
- Your Xi 400 or Xi 410 camera plugged in and visible in PIX Connect
- A Gmail account you can use for sending alerts

---

## Step 1 — Create a Gmail App Password

Gmail blocks plain password logins from scripts. You need a one-time "App Password" instead.

1. Go to your Google Account → **Security** → **2-Step Verification** (must be enabled first)
2. At the bottom of that page, click **App passwords**
3. Choose app: **Mail**, device: **Windows Computer** → click **Generate**
4. Copy the 16-character password shown (e.g. `abcd efgh ijkl mnop`) — you only see it once

---

## Step 2 — Set up PIX Connect

1. Open PIX Connect with your camera connected
2. Draw at least one **Measure Area** on the thermal image (right-click the image → Insert → Measure Area)
3. Name the measure area something clear (e.g. `Hot Spot`)
4. If you want to use PIX Connect's built-in alarm thresholds: set them in PIX Connect under the measure area properties
5. Leave PIX Connect running — the daemon connects to it via its IPC interface

---

## Step 3 — Create your config file

Copy the example config and edit it:

```
copy config\monitor.example.json config\monitor.json
```

Open `config\monitor.json` and fill in your values. For a single camera this is all you need:

```json
{
  "sdk_dll_path": "C:\\Program Files\\Optris GmbH\\PIX Connect\\ImagerIPC2x64.dll",

  "email": {
    "smtp_host": "smtp.gmail.com",
    "smtp_port": 465,
    "username": "your.address@gmail.com",
    "password_env": "GMAIL_APP_PASSWORD",
    "from_address": "your.address@gmail.com",
    "to_addresses": ["recipient@example.com"],
    "subject_prefix": "Optris Alarm",
    "use_ssl": true
  },

  "runtime": {
    "poll_interval_seconds": 1.0,
    "consecutive_samples": 2,
    "repeat_alert_interval_seconds": 1800.0,
    "clear_alerts": true,
    "log_level": "INFO"
  },

  "cameras": [
    {
      "name": "My Camera",
      "measure_areas": [
        {
          "name": "Hot Spot",
          "direction": "high",
          "threshold_source": "pix_connect"
        }
      ]
    }
  ]
}
```

**Key fields to change:**
| Field | What to put |
|---|---|
| `username` | Your Gmail address |
| `to_addresses` | Who receives the alert emails (can be yourself) |
| `cameras[0].name` | Any label you want for this camera |
| `measure_areas[0].name` | Must exactly match the measure area name in PIX Connect |

**If you want custom thresholds instead of using PIX Connect's settings**, change `threshold_source` to `"custom"` and add a `thresholds` block:

```json
"threshold_source": "custom",
"thresholds": {
  "high_alarm_c": 120.0,
  "high_pre_alarm_c": 100.0
}
```

---

## Step 4 — Set the Gmail App Password as an environment variable

Open a Command Prompt and run (replacing the password with the one from Step 1, no spaces):

```cmd
setx GMAIL_APP_PASSWORD "fdjy omej lwso ytyt"
```

Close and reopen the Command Prompt after running this — `setx` only applies to new windows.

---

## Step 5 — Validate your config

Check that the config file is valid and the DLL can be found, without actually connecting to the camera:

```cmd
python run_daemon.py --config config\monitor.json --validate
```

You should see something like:
```
INFO: Config loaded successfully from ...
INFO: SDK DLL: C:\Program Files\Optris GmbH\PIX Connect\ImagerIPC2x64.dll
INFO: Camera configured: My Camera (instance=<default>)
```

If you see an error, fix the config file and re-run validate until it passes.

---

## Step 6 — Start the daemon

Make sure PIX Connect is open and your camera is live, then run:

```cmd
python run_daemon.py --config config\monitor.example.json
```

You should see:
```
INFO: Connected to My Camera (instance=<default>).
INFO: Monitoring started for 1 camera(s).
```

The daemon now polls the camera every second. When a threshold is exceeded it sends an email, and again every 30 minutes while the alarm is active. When the temperature returns to normal it sends a "cleared" email.

Press **Ctrl+C** to stop.

---

## Troubleshooting

| Problem | Likely cause |
|---|---|
| `Connect SDK DLL not found` | PIX Connect is not installed, or installed to a different path — update `sdk_dll_path` in config |
| `Timed out waiting for IPC init` | PIX Connect is not running, or the camera is not connected |
| `measure area name ... was not found` | The `name` in your config doesn't exactly match what's in PIX Connect — check spelling and case |
| `GMAIL_APP_PASSWORD is not set` | Re-run `setx`, then open a **new** Command Prompt |
| Email not delivered | Check Gmail's Sent folder; make sure 2-Step Verification is on and you used an App Password, not your regular password |
