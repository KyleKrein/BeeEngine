# BeeEngine
This is a General Purpose 2D (and in the future 3D also) cross-platform game engine, that is in early stages of development.

## Features

- Cross-platform
- Fully featured Editor
- Written in C++, but games can be written in C#
- Can be used for standalone applications (BeeEngineEditor and BeeLocalization apps are powered by the BeeEngine)
- Projects
- Asset management system
- Uses Vulkan as renderer backend
- ECS with entt backend

## Platforms and Building

### Supported Platforms
Windows and MacOS are main platforms for now. Linux hasn't been tested, therefore it might not build or run. Mobile platforms are in development.

Tested on Apple-Clang compiler on MacOS and MSVC compiler on Windows.

### Building
To build BeeEngine you need to:
1) Clone github repository recursively:
~~~
git clone https://github.com/KyleKrein/BeeEngine.git --recursive
~~~
2) Run install script with admin rights from terminal:

*MacOS*:
~~~
./macos_install.sh
~~~
*Linux*:
~~~
./linux_install.sh
~~~
*Windows*:
~~~
windows_install.bat
~~~

3) Download and install Vulkan SDK from https://vulkan.lunarg.com/sdk/home

4) Open project in IDE that supports CMake and CMakePresets.json

5) Load CMakeLists and wait. You might need to reload it several times in order to fetch all dependencies and generate all code

6) Choose one of the targets to build: BeeEngine to get a static lib or BeeEngineEditor to build editor app, that uses BeeEngine