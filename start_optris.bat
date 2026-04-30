@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "PYTHON=%PROJECT_DIR%.venv\Scripts\python.exe"
set "CONFIG=%PROJECT_DIR%config\monitor.example.json"

echo Starting Optris alert daemon (launches PIX Connect, configures IPC, monitors cameras)...
"%PYTHON%" "%PROJECT_DIR%run_daemon.py" --config "%CONFIG%"

endlocal
