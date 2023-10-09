#!/bin/bash
if ! command -v python3 &>/dev/null; then
    echo "Installing Python..."
    sudo apt update
    sudo apt install -y python3.9
fi
python3 --version
echo "Running Setup.py..."
python3 Setup.py
