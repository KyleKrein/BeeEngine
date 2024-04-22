
# Installation Guide for BeeEngine

## System Requirements
### Supported Platforms
- **Windows**: Fully supported for development.
- **MacOS**: Fully supported for development.
- **Linux**: Not officially supported; build might fail.
- **Mobile Platforms**: In development; not available yet.

### Software Requirements
- **Vulkan SDK**: Necessary for graphics rendering. Download from [Vulkan SDK Official Page](https://vulkan.lunarg.com/sdk/home).
- **CMake**: Required for building the project.
- **IDE with CMake Support**: Recommended IDEs include CLion, Visual Studio, or Visual Studio Code.

## Installation Steps

### Downloading the Source Code
Currently, BeeEngine does not have a binary installer available. You need to build it from the source:
```bash
git clone https://github.com/KyleKrein/BeeEngine.git --recursive
```

### Running Installation Scripts
Depending on your operating system, run the appropriate script with admin rights:

**MacOS**:
```bash
./macos_install.sh
```

**Windows**:
```bash
windows_install.bat
```

**Linux** (not officially supported):
```bash
./linux_install.sh
```

### Building the Engine
1. Open your IDE and load the project using `CMakeLists.txt`.
2. Ensure `CMakePresets.json` is correctly set up in your IDE.
3. Trigger a CMake configuration. You might need to reload configurations multiple times to resolve dependencies.

### Build Targets
- **BeeEngine**: Builds the static library.
- **BeeEngineEditor**: Builds the editor application utilizing BeeEngine.

## Future Installation Methods
In the future, a pre-compiled installer will be available for easier setup. This section will be updated with download links once available.

## Verifying Installation
After building, run the BeeEngineEditor to verify the engine is set up correctly. Ensure all functionalities are accessible and performing as expected.

Thank you for installing BeeEngine!
