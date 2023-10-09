@echo off
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo Python not found. Downloading Python installer...
    powershell -Command "Invoke-WebRequest -Uri 'https://www.python.org/ftp/python/3.9.7/python-3.9.7-amd64.exe' -OutFile 'python-installer.exe'"
    python-installer.exe /quiet InstallAllUsers=1 PrependPath=1
    del python-installer.exe
)
python --version
echo Running Setup.py...
python Setup.py