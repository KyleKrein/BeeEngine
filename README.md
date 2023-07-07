# BeeEngine
This is a General Purpose 2D (and in the future 3D also) crossplatform game engine, that is on early stages of development.

### Supported Platforms
Windows and MacOS are main platforms for now. Linux haven't been tested, therefore it might not build or run. Mobile platorms are in development.

Tested on Clang and Apple-Clang compilers.

### Building
To build BeeEngine you need to have installed:
- Python 3
- CMake 3.25

1) Clone github repository recursively:
~~~
git clone https://github.com/KyleKrein/BeeEngine.git --recursive
~~~
2) Open project in IDE that supports CMake and CMakePresets.json
3) Load CMakeLists and wait. You might need to reload it several times in order to fetch all dependencies and generate all code
4) Choose one of the targets to build: BeeEngine to get a static lib or Sandbox2D to build example app, that uses BeeEngine

