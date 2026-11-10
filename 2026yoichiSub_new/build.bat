@echo off
setlocal
pushd "%~dp0"
"C:\mbedOffline44\WinPython\python-2.7.13\python.exe" "C:\mbedOffline44\WinPython\python-2.7.13\Scripts\mbed-script.py" compile -m NUCLEO_F446RE -t GCC_ARM --source . --source "C:\mbedOffline44\Datas\MbedOS2"
set "build_result=%ERRORLEVEL%"
popd
exit /b %build_result%
