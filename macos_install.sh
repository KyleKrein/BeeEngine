#!/bin/bash
if ! command -v python3 &>/dev/null; then
    echo "Python not found. Downloading Python installer..."
    curl -O "https://www.python.org/ftp/python/3.9.7/python-3.9.7-macosx10.9.pkg"
    sudo installer -pkg python-3.9.7-macosx10.9.pkg -target /
    rm python-3.9.7-macosx10.9.pkg
fi
python3 --version
echo "Running Setup.py..."
python3 Setup.py
