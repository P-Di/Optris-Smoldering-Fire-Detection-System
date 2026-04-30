# Optris PIX Connect Alert Daemon

This project monitors one or more Optris PIX Connect instances, reads measure-area temperatures through the Connect SDK, evaluates alarm state, and sends email alerts through Gmail or another SMTP server.

It is designed around the vendor flow that Optris documents:

- PIX Connect must be running for Connect SDK / IPC.
- Multiple cameras are handled by multiple PIX Connect instances and instance names.
- The SDK can read alarm settings for measure areas.
- The SDK can also expose fast per-area values and alarm bytes through `OnNewAreasBuffer` / `GetAreas`, but this implementation stays on the simpler polling path from Python and reuses PIX Connect thresholds where you want it to.

## What This Does

- Connects to one or more PIX Connect instances by instance name.
- Looks up configured measure areas by name or index.
- Reads the current measure-area temperature continuously.
- Reads alarm thresholds from PIX Connect, or uses custom thresholds from JSON.
- Sends an email when a measure area enters alarm.
- Optionally sends pre-alarm emails.
- Optionally sends a clear email when the area returns to normal.
- Optionally triggers a PIX Connect snapshot and attaches it to the email.
- Tries to reconnect if PIX Connect restarts or drops IPC.

## What This Does Not Do

- It does not replace PIX Connect.
- It does not render its own live video viewer.
- It does not currently consume the fast `GetAreas` alarm-state buffer directly.

That last point is deliberate. The SDK documentation confirms that the fast buffer includes per-area alarm bytes, but the simple polling calls are easier to drive safely from pure Python and are usually enough for alerting. If you later need full device-frame-rate alarm reads, this codebase is a clean starting point for swapping in the callback or `GetAreas` path.

## Recommended Architecture

For Xi 400 and Xi 410 together, the cleanest common path is:

1. Run one PIX Connect instance per camera.
2. Name each instance with `/Name=<InstanceName>`.
3. In each PIX Connect instance:
   Create the measure area.
   Configure the alarm.
   Enable `Tools -> Configuration -> External Communication -> Connect SDK (IPC)`.
4. Run this daemon against those named instances.

If you have only Xi 410 units and only need temperature data, Optris also documents a direct temperature / Easy Comm path for Xi 410. That can bypass PIX Connect. I did not build around that here because it would not cover Xi 400 in the same way, and you said feed visibility is still useful.

## Live View Options

The simplest options are:

- Keep PIX Connect visible or minimized and let this daemon run alongside it.
- Run PIX Connect mostly in the background and use its built-in Web Server for occasional viewing in a browser.
- If you later want a custom Windows operator UI, the Connect SDK supports embedding the PIX Connect video window, but that is a separate application effort.

If operators need to watch the feed regularly, keep PIX Connect visible. If this machine is mostly unattended, run PIX Connect in the background and use the daemon plus snapshots or the Web Server.

## Gmail Setup

For Gmail, use an app password, not your normal mailbox password.

1. Turn on 2-step verification for the Gmail account.
2. Generate an app password in Google Account settings.
3. Set it in an environment variable before launching the daemon:

```powershell
$env:GMAIL_APP_PASSWORD="your-16-char-app-password"
```

## Config

Copy [config/monitor.example.json](/c:/Users/paul/OneDrive/Desktop/SFD/config/monitor.example.json) to `config/monitor.json` and edit:

- `sdk_dll_path`: path to `ImagerIPC2x64.dll`
- `email`: SMTP settings
- `runtime`: polling, reconnect, logging
- `cameras[*].instance_name`: PIX Connect instance name
- `cameras[*].launch_command`: optional command to start PIX Connect
- `cameras[*].measure_areas[*]`: what to watch

Each measure area can use:

- `threshold_source: "pix_connect"` to read thresholds from PIX Connect
- `threshold_source: "custom"` to use values from the JSON file

`direction` defaults to the typical use case of a high-temperature alarm in the example. Set it to:

- `high`
- `low`
- `both`

## Running

The machine in this workspace has a usable interpreter at `C:\msys64\mingw64\bin\python.exe`, so the commands below use that.

Validate config only:

```powershell
C:\msys64\mingw64\bin\python.exe run_daemon.py --config config\monitor.example.json --validate
```

Run the daemon:

```powershell
$env:GMAIL_APP_PASSWORD="your-16-char-app-password"
C:\msys64\mingw64\bin\python.exe run_daemon.py --config config\monitor.json
```

## Startup / Daemon Mode on Windows

For unattended use, Task Scheduler is usually the practical choice.

Recommended action:

- Program/script:

```text
C:\msys64\mingw64\bin\python.exe
```

- Add arguments:

```text
C:\Users\paul\OneDrive\Desktop\SFD\run_daemon.py --config C:\Users\paul\OneDrive\Desktop\SFD\config\monitor.json
```

- Start in:

```text
C:\Users\paul\OneDrive\Desktop\SFD
```

Set the task to:

- Run whether user is logged on or not
- Restart on failure
- Start at boot

## Notes on Alarm Semantics

PIX Connect itself owns the camera-side alarm setup. This daemon can reuse those thresholds, but it evaluates the current temperature in Python rather than reading the fast per-area alarm byte buffer directly.

That means:

- The threshold numbers come from PIX Connect when `threshold_source` is `pix_connect`.
- The daemon decides whether the live temperature is in normal, pre-alarm, or alarm state.

If you want exact byte-for-byte parity with the vendor alarm state buffer, the next step would be replacing the polling path with `GetAreas` or `OnNewAreasBuffer`.

## Testing

The included tests cover the alarm-state evaluation and dedupe logic:

```powershell
C:\msys64\mingw64\bin\python.exe -m unittest discover -s tests -v
```

These tests do not require the Optris DLL or cameras.
