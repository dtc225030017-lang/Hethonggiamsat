@echo off
setlocal
cd /d "%~dp0"

set "PIO=%USERPROFILE%\.platformio\penv\Scripts\platformio.exe"
if not exist "%PIO%" set "PIO=platformio"

echo Building and uploading Hethonggiamsat ESP32...
"%PIO%" run --target upload
if errorlevel 1 (
    echo.
    echo Upload failed. Check the USB cable and COM port, then try again.
    pause
    exit /b 1
)

echo.
echo Upload completed successfully.
pause
