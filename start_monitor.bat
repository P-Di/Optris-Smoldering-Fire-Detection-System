@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "PYTHON=%PROJECT_DIR%.venv\Scripts\python.exe"
set "CONFIG=%PROJECT_DIR%config\monitor.json"

if not exist "%PYTHON%" (
    echo [ERROR] Virtual environment not found at %PROJECT_DIR%.venv
    echo Run:  python -m venv .venv  ^&^&  .venv\Scripts\pip install -r requirements.txt
    pause
    exit /b 1
)

if not exist "%CONFIG%" (
    echo [ERROR] Config not found: %CONFIG%
    pause
    exit /b 1
)

echo Starting Optris Thermal Monitor (watchdog enabled)...
"%PYTHON%" "%PROJECT_DIR%watchdog.py" "%CONFIG%"

endlocal
