"""Automate PIX Connect settings to ensure Connect SDK (IPC) is enabled.

After launching a PIX Connect instance, call configure_pix_connect_instance()
with the Popen handle. It opens the Settings dialog via pywinauto, navigates
to the External Communication tab, and selects Connect SDK.

The function is idempotent — if Connect SDK is already selected it does nothing
harmful. If pywinauto is unavailable or automation fails the function logs a
warning with manual instructions and returns False so the caller can decide
whether to abort or proceed.
"""
from __future__ import annotations

import logging
import subprocess
import time
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    pass

LOG = logging.getLogger("optris_alert_daemon")

# Text fragments used to identify controls (checked case-insensitively).
# PIX Connect ships in multiple languages; include German equivalents.
_SETTINGS_DIALOG_TITLES = (
    "settings", "einstellungen", "options", "optionen", "configuration", "konfiguration",
)
_EXT_COMM_TAB_TEXTS = (
    "external communication", "externe kommunikation", "external", "communication", "ipc",
)
_CONNECT_SDK_TEXTS = (
    "connect sdk", "sdk", "ipc2", "ipc ", "connect sdk (ipc2)",
)
_OK_BUTTON_TEXTS = ("ok", "apply", "übernehmen", "anwenden", "accept")


def configure_pix_connect_instance(
    proc: subprocess.Popen,
    instance_name: str | None,
    *,
    window_ready_timeout: float = 25.0,
    configure_timeout: float = 15.0,
) -> bool:
    """Enable Connect SDK in PIX Connect Settings > External Communication.

    Args:
        proc: The Popen object returned when PIX Connect was launched.
        instance_name: The /Name= argument used at launch (for log messages).
        window_ready_timeout: Seconds to wait for the PIX Connect window.
        configure_timeout: Seconds to spend on UI automation attempts.

    Returns:
        True if successful, False if automation could not complete (manual
        configuration required).
    """
    label = instance_name or "default"
    try:
        import pywinauto  # noqa: F401 — check import before proceeding
        from pywinauto import Application
        from pywinauto.findwindows import ElementNotFoundError
    except ImportError:
        LOG.warning(
            "PIX Connect [%s]: pywinauto is not installed. "
            "Cannot auto-configure External Communication. "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
        )
        return False

    # Connect to the process launched by Popen
    pid = proc.pid
    LOG.info("PIX Connect [%s]: waiting for window (PID %d)…", label, pid)
    app = _wait_for_app(pid, window_ready_timeout)
    if app is None:
        LOG.warning(
            "PIX Connect [%s]: timed out waiting for process window. "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
        )
        return False

    try:
        return _configure_settings(app, label, configure_timeout)
    except Exception as exc:
        LOG.warning(
            "PIX Connect [%s]: UI automation error (%s). "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
            exc,
        )
        return False


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------

def _wait_for_app(pid: int, timeout: float):
    """Return a connected pywinauto Application for pid, or None on timeout."""
    from pywinauto import Application
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        try:
            app = Application(backend="win32").connect(process=pid, timeout=2)
            # Verify we can reach the top window
            win = app.top_window()
            win.wait("exists", timeout=2)
            return app
        except Exception:
            time.sleep(1)
    return None


def _configure_settings(app, label: str, timeout: float) -> bool:
    """Open Settings dialog and enable Connect SDK.  Returns True on success."""
    from pywinauto.keyboard import send_keys

    main_win = app.top_window()
    main_win.wait("ready", timeout=10)
    main_win.set_focus()
    time.sleep(0.3)

    # --- Open the settings dialog ---
    # Strategy 1: F4 shortcut (common in Delphi/VCL apps)
    main_win.type_keys("{F4}", pause=0.05)
    time.sleep(0.8)
    dlg = _find_settings_dialog(app)

    # Strategy 2: Try menu bar — cycle through top-level menus looking for settings
    if dlg is None:
        main_win.set_focus()
        _try_menu_for_settings(main_win)
        time.sleep(0.8)
        dlg = _find_settings_dialog(app)

    # Strategy 3: Alt+S, Alt+O, Alt+E (common menu accelerators)
    for accel in ("%s", "%o", "%e"):
        if dlg is not None:
            break
        main_win.set_focus()
        main_win.type_keys(accel, pause=0.05)
        time.sleep(0.6)
        dlg = _find_settings_dialog(app)
        if dlg is None:
            # The menu may have opened without showing a dialog yet; press Enter on first item
            send_keys("{ENTER}")
            time.sleep(0.6)
            dlg = _find_settings_dialog(app)

    if dlg is None:
        LOG.warning(
            "PIX Connect [%s]: could not open Settings dialog after multiple attempts. "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
        )
        return False

    LOG.debug("PIX Connect [%s]: Settings dialog found: %r", label, dlg.window_text())

    # --- Find External Communication tab ---
    tab_clicked = _click_tab(dlg, _EXT_COMM_TAB_TEXTS)
    if not tab_clicked:
        LOG.warning(
            "PIX Connect [%s]: External Communication tab not found in Settings dialog. "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
        )
        _dismiss_dialog(dlg)
        return False

    time.sleep(0.4)

    # --- Find and enable Connect SDK ---
    enabled = _enable_connect_sdk_control(dlg)
    if not enabled:
        LOG.warning(
            "PIX Connect [%s]: Connect SDK control not found on External Communication tab. "
            "Please manually enable Settings > External Communication > Connect SDK.",
            label,
        )
        _dismiss_dialog(dlg)
        return False

    time.sleep(0.2)

    # --- Confirm / close ---
    _click_ok(dlg)
    LOG.info(
        "PIX Connect [%s]: External Communication > Connect SDK enabled successfully.",
        label,
    )
    return True


def _find_settings_dialog(app):
    """Return the first open dialog whose title matches a settings pattern, or None."""
    try:
        for win in app.windows():
            title = (win.window_text() or "").lower()
            if any(t in title for t in _SETTINGS_DIALOG_TITLES):
                return win
    except Exception:
        pass
    return None


def _try_menu_for_settings(main_win) -> None:
    """Walk the menu bar looking for a settings-like entry and click it."""
    try:
        menu = main_win.menu()
        if menu is None:
            return
        for i in range(menu.item_count()):
            item = menu.item_by_index(i)
            text = (item.text() or "").lower().strip("&")
            # Click menus that sound like they might contain settings
            if any(kw in text for kw in ("extra", "setting", "option", "tool", "einstellung")):
                item.click_input()
                time.sleep(0.4)
                # Now look for a submenu item with settings text
                try:
                    sub = main_win.menu()
                    for j in range(sub.item_count()):
                        sub_item = sub.item_by_index(j)
                        sub_text = (sub_item.text() or "").lower().strip("&")
                        if any(kw in sub_text for kw in ("setting", "option", "einstellung", "konfigur")):
                            sub_item.click_input()
                            return
                except Exception:
                    pass
                return
    except Exception:
        pass


def _click_tab(dlg, target_texts: tuple[str, ...]) -> bool:
    """Click the tab whose text matches any of target_texts. Returns True on success."""
    try:
        for child in dlg.children():
            class_name = child.friendly_class_name().lower()
            if "tab" in class_name:
                # Tab control — iterate its items
                try:
                    for i in range(child.tab_count()):
                        tab_text = (child.tab_text(i) or "").lower()
                        if any(t in tab_text for t in target_texts):
                            child.select(i)
                            return True
                except Exception:
                    pass
            # Also check if child IS a tab page/item
            text = (child.window_text() or "").lower()
            if any(t in text for t in target_texts):
                try:
                    child.click_input()
                    return True
                except Exception:
                    pass
    except Exception:
        pass
    return False


def _enable_connect_sdk_control(dlg) -> bool:
    """Find and select the Connect SDK radio button or checkbox. Returns True if found."""
    try:
        for child in dlg.descendants():
            text = (child.window_text() or "").lower()
            class_name = child.friendly_class_name().lower()
            if any(t in text for t in _CONNECT_SDK_TEXTS):
                if "radio" in class_name or "check" in class_name or "button" in class_name:
                    try:
                        # For radio buttons: check if already selected
                        if hasattr(child, "get_check_state") and child.get_check_state() == 1:
                            LOG.debug("Connect SDK already selected.")
                            return True
                        child.click_input()
                        return True
                    except Exception:
                        try:
                            child.select()
                            return True
                        except Exception:
                            pass
    except Exception:
        pass
    return False


def _click_ok(dlg) -> None:
    """Click the OK or Apply button to confirm settings."""
    try:
        for child in dlg.children():
            text = (child.window_text() or "").lower()
            if any(t == text for t in _OK_BUTTON_TEXTS):
                child.click_input()
                return
        # Fallback: press Enter to confirm
        from pywinauto.keyboard import send_keys
        send_keys("{ENTER}")
    except Exception:
        pass


def _dismiss_dialog(dlg) -> None:
    """Close the settings dialog without saving (Escape)."""
    try:
        from pywinauto.keyboard import send_keys
        send_keys("{ESC}")
    except Exception:
        pass
