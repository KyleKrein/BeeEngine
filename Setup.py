import subprocess
import platform
import re
import logging
import os
import glob

logging.basicConfig(level=logging.INFO)


def check_version_installed(command, version_required=None):
    try:
        result = subprocess.run([command, '--version'], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        version_text = result.stdout or result.stderr
        version_match = re.search(r'(\d+\.\d+(\.\d+)?)', version_text)
        if version_match:
            version_installed = version_match.group(1)
            if version_required:
                version_installed_list = list(map(int, version_installed.split('.')))
                version_required_list = list(map(int, version_required.split('.')))
                if version_installed_list >= version_required_list:
                    logging.info(f"{command} already installed: version {version_installed}")
                    return True
                else:
                    return False
            logging.info(f"{command} already installed: version {version_installed}")
            return True
        return False
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False


def check_and_install_choco():
    if not check_version_installed('choco'):
        logging.info("Installing Chocolatey...")
        subprocess.run([
                           '@"%SystemRoot%\\System32\\WindowsPowerShell\\v1.0\\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command " [System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString(\'https://community.chocolatey.org/install.ps1\'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\\chocolatey\\bin"'],
                       shell=True)


def check_and_install_brew():
    if not check_version_installed('brew'):
        logging.info("Installing Homebrew...")
        subprocess.run(
            ['/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'],
            shell=True)


def check_xcode_command_line_tools_installed():
    try:
        result = subprocess.run(['xcode-select', '-p'], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.stdout:
            logging.info("Xcode command line tools are already installed.")
            return True
    except subprocess.CalledProcessError:
        return False


def install_xcode_command_line_tools():
    if not check_xcode_command_line_tools_installed():
        try:
            logging.info("Installing Xcode command line tools...")
            subprocess.run(['xcode-select', '--install'], check=True)
        except subprocess.CalledProcessError:
            logging.error("Failed to install Xcode Command Line Tools. Please try installing manually.")


def install_dependencies():
    os_name = platform.system()

    logging.info(f"Detected OS: {os_name}")

    if os_name == 'Windows':
        check_and_install_choco()
        if not check_version_installed('git'):
            logging.info("Installing Git...")
            subprocess.run(['choco', 'install', 'git'])
        if not check_version_installed('cmake', '3.26'):
            logging.info("Installing CMake...")
            subprocess.run(['choco', 'install', 'cmake'])
        if not check_version_installed('cl'):
            # Поиск папки установки MSVC
            vs_installation_paths = glob.glob("C:/Program Files/Microsoft Visual Studio/2022/*/VC/Tools/MSVC/*") + glob.glob("C:/Program Files (x86)/Microsoft Visual Studio/2022/*/VC/Tools/MSVC/*") + glob.glob("C:/Program Files/Microsoft Visual Studio/2022/*/BuildTools/VC/Tools/MSVC/*") + glob.glob("C:/Program Files (x86)/Microsoft Visual Studio/2022/*/BuildTools/VC/Tools/MSVC/*")
            if not vs_installation_paths:
                logging.info("MSVC Build Tools not found. Installing...")
                subprocess.run(['choco', 'install', 'visualstudio2022buildtools'])
            else:
                logging.info(f"MSVC Build Tools found at {vs_installation_paths[0]}")
        if not check_version_installed('ccache'):
            logging.info("Installing ccache...")
            subprocess.run(["choco", "install", "ccache"], check=True)
        if not check_version_installed('ninja'):
            logging.info("Installing Ninja...")
            subprocess.run(["choco", "install", "ninja"], check=True)
        if not check_version_installed('dotnet'):
            logging.info("Installing .NET SDK...")
            subprocess.run(["choco", "install", "dotnet-sdk"], check=True)

    elif os_name == 'Darwin':
        check_and_install_brew()
        if not check_version_installed('git'):
            logging.info("Installing Git...")
            subprocess.run(['brew', 'install', 'git'])
        if not check_version_installed('cmake', '3.26'):
            logging.info("Installing CMake...")
            subprocess.run(['brew', 'install', 'cmake'])
        install_xcode_command_line_tools()
        if not check_version_installed('ccache'):
            logging.info("Installing ccache...")
            subprocess.run(["brew", "install", "ccache"], check=True)
        if not check_version_installed('ninja'):
            logging.info("Installing Ninja...")
            subprocess.run(["brew", "install", "ninja"], check=True)
        if not check_version_installed('dotnet'):
            logging.info("Installing .NET SDK...")
            subprocess.run(["brew", "install", "dotnet"], check=True)
        if not check_version_installed('autoconf'):
            logging.info("Installing autoconf...")
            subprocess.run(['brew', 'install', 'autoconf'])
        if not check_version_installed('automake'):
            logging.info("Installing automake...")
            subprocess.run(['brew', 'install', 'automake'])
        if not check_version_installed('autoconf-archive'):
            logging.info("Installing autoconf-archive...")
            subprocess.run(['brew', 'install', 'autoconf-archive'])


def init_git_submodules():
    try:
        logging.info("Initializing Git submodules...")
        subprocess.run(['git', 'submodule', 'update', '--init', '--recursive'], check=True)
        logging.info("Git submodules initialized successfully.")
    except subprocess.CalledProcessError:
        logging.error("Failed to initialize Git submodules. You might want to try running 'git submodule update --init --recursive' manually.")


if __name__ == '__main__':
    install_dependencies()
    init_git_submodules()
