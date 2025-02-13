cmake_minimum_required(VERSION 3.25)
project(
        BeeEngine
        VERSION 0.1
        DESCRIPTION "A game engine written in C++"
        LANGUAGES CXX C
)
#set(CMAKE_CXX_STANDARD 23)
#set(BEE_COMPILE_GLFW OFF)
#set(BEE_COMPILE_WEBGPU ON)

set(BEE_ENABLE_RUNTIME_COMPILER OFF)

set(SOURCE_FILES src/BeeEngine.cpp src/Core/TypeDefines.h src/Core/EntryPoint.cpp src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/ApplicationProperties.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/Renderer/RendererAPI.h src/Platform/ImGui/ImGuiController.h src/Core/Layer.cpp src/Renderer/RendererAPI.cpp src/Core/Color4.cpp src/Core/Color4.h src/Renderer/BufferLayout.cpp src/Renderer/BufferLayout.h src/Renderer/Texture.cpp src/Renderer/Texture.h src/Core/Events/Event.cpp src/Allocator/Allocator.h src/Debug/MemoryProfiler.cpp src/Debug/MemoryProfiler.h src/Debug/DebugLayer.cpp src/Debug/DebugLayer.h src/Core/SharedPointer.cpp src/Core/SharedPointer.h src/Renderer/FrameBuffer.cpp src/Renderer/FrameBuffer.h src/Core/ResourceManager.cpp src/Core/ResourceManager.h src/Core/Cameras/ICamera.cpp src/Core/Cameras/ICamera.h src/Renderer/RectangleProperties.h src/Core/Time.cpp src/Core/Time.h src/Debug/Timer.h src/Debug/Instrumentor.h src/Core/CodeSafety/Expects.h src/Gui/ImGui/FpsCounter.cpp src/Gui/ImGui/FpsCounter.h src/Scene/Scene.cpp src/Scene/Scene.h src/Scene/EntityID.h src/Scene/Components.h src/Scene/Entity.cpp src/Scene/Entity.h src/Core/Cameras/Camera.cpp src/Core/Cameras/Camera.h src/Scene/SceneCamera.cpp src/Scene/SceneCamera.h src/Scene/ScriptableEntity.cpp src/Scene/ScriptableEntity.h src/Platform/ImGui/ImGuiController.cpp vendor/Incbin/incbin.h Assets/EmbeddedResources.h src/Gui/ImGuiFonts.h src/Scene/SceneSerializer.cpp src/Scene/SceneSerializer.h src/source_location.h src/Property.h src/Utils/FileDialogs.h src/Core/Math/Math.h src/Core/Math/Math.cpp src/Renderer/EditorCamera.cpp src/Renderer/EditorCamera.h Assets/EmbeddedResources.cpp src/Core/CodeSafety/DebugLog.cpp src/Core/CodeSafety/DebugLog.h src/Core/OsPlatform.h src/Renderer/RenderAPI.h src/Renderer/Surface.cpp src/Renderer/Surface.h src/Renderer/DeviceID.cpp src/Renderer/DeviceID.h src/Renderer/SwapChain.cpp src/Renderer/SwapChain.h src/Platform/Vulkan/VulkanGraphicsDevice.cpp src/Platform/Vulkan/VulkanGraphicsDevice.h src/Platform/Vulkan/VulkanSwapChain.cpp src/Platform/Vulkan/VulkanSwapChain.h src/Renderer/Instance.cpp src/Renderer/Instance.h src/Platform/Vulkan/VulkanInstance.cpp src/Platform/Vulkan/VulkanInstance.h src/Renderer/QueueFamilyIndices.cpp src/Renderer/QueueFamilyIndices.h src/FileSystem/File.cpp src/FileSystem/File.h src/Platform/ImGui/ImGuiControllerVulkan.h src/Platform/ImGui/ImGuiControllerVulkan.cpp src/Utils/ShaderConverter.h src/Utils/ShaderConverter.cpp vendor/VulkanMemoryAllocator/vk_mem_alloc.h src/Core/CodeSafety/NotNull.h src/Core/CodeSafety/BoundsChecking.h src/Platform/Vulkan/VulkanBuffer.h src/Renderer/Vertex.h src/Core/DeletionQueue.cpp src/Core/DeletionQueue.h src/Utils/ModelLoader.cpp src/Utils/ModelLoader.h src/Platform/Vulkan/Utils.h src/Platform/Vulkan/Utils.cpp src/Renderer/AssetManager.cpp src/Renderer/AssetManager.h src/Renderer/Mesh.h src/Renderer/Mesh.cpp src/Renderer/Material.cpp src/Renderer/Material.h src/Windowing/WindowHandler/SDLWindowHandler.cpp src/Windowing/WindowHandler/SDLWindowHandler.h src/Platform/WebGPU/WebGPUInstance.cpp src/Platform/WebGPU/WebGPUInstance.h src/Platform/WebGPU/WebGPUGraphicsDevice.cpp src/Platform/WebGPU/WebGPUGraphicsDevice.h src/Platform/WebGPU/WebGPUSwapchain.cpp src/Platform/WebGPU/WebGPUSwapchain.h src/Platform/WebGPU/WebGPURendererAPI.cpp src/Platform/WebGPU/WebGPURendererAPI.h src/Renderer/CommandBuffer.h src/Platform/WebGPU/WebGPUCommandBuffer.cpp src/Platform/WebGPU/WebGPUCommandBuffer.h src/Platform/ImGui/ImGuiControllerWebGPU.h src/Platform/ImGui/ImGuiControllerWebGPU.cpp src/Renderer/RenderPass.h src/Platform/WebGPU/WebGPUPipeline.cpp src/Platform/WebGPU/WebGPUPipeline.h src/Renderer/Pipeline.h src/Renderer/ShaderTypes.h src/Renderer/ShaderModule.h src/Renderer/ShaderModule.cpp src/Platform/WebGPU/WebGPUShaderModule.cpp src/Platform/WebGPU/WebGPUShaderModule.h src/Renderer/Pipeline.cpp src/Platform/WebGPU/WebGPUMesh.cpp src/Platform/WebGPU/WebGPUMesh.h src/Renderer/UniformBuffer.cpp src/Renderer/UniformBuffer.h src/Platform/WebGPU/WebGPUUniformBuffer.cpp src/Platform/WebGPU/WebGPUUniformBuffer.h src/Renderer/InstancedBuffer.h src/Platform/WebGPU/WebGPUInstancedBuffer.cpp src/Platform/WebGPU/WebGPUInstancedBuffer.h src/Core/RestartApplication.h src/Renderer/Model.h src/Platform/WebGPU/WebGPUMaterial.cpp src/Platform/WebGPU/WebGPUMaterial.h src/Renderer/Model.cpp src/Platform/WebGPU/WebGPUModel.cpp src/Platform/WebGPU/WebGPUTexture2D.cpp src/Platform/WebGPU/WebGPUTexture2D.h vendor/Implementations.cpp src/Renderer/BindingSet.cpp src/Renderer/BindingSet.h src/Renderer/IBindable.h src/Renderer/MaterialDescriptor.h src/Core/ValueType.h src/Platform/WebGPU/WebGPUBindingSet.cpp src/Platform/WebGPU/WebGPUBindingSet.h src/Platform/WebGPU/WebGPUBufferPool.cpp src/Platform/WebGPU/WebGPUBufferPool.h src/Renderer/RenderingQueue.cpp src/Renderer/RenderingQueue.h src/Renderer/InstancedBuffer.cpp src/Renderer/RendererStatistics.h src/Gui/ImGui/RendererStatisticsGUI.cpp src/Gui/ImGui/RendererStatisticsGUI.h src/Platform/WebGPU/WebGPUFramebuffer.cpp src/Platform/WebGPU/WebGPUFramebuffer.h src/Core/FramePtr.cpp src/Core/FramePtr.h src/Scene/INativeScriptRegistry.h src/Scene/NativeScriptFactory.cpp src/Scene/NativeScriptFactory.h src/Scene/INativeScriptFactory.h src/Utils/DynamicLibrary.cpp src/Utils/DynamicLibrary.h src/Core/Logging/GameLogger.cpp src/Core/Logging/GameLogger.h src/Scene/DefaultNativeScript.h src/Core/UUID.cpp src/Core/UUID.h src/Scene/Components.cpp src/Allocator/AllocatorStatistics.h src/Allocator/AllocatorStatistics.h
        src/Renderer/Font.cpp
        src/Renderer/Font.h
        src/Renderer/MSDFData.h
        src/Renderer/TextRenderingConfiguration.h
        src/Threading/ThreadPool.h
        src/Core/AssetManagement/Asset.h
        src/Core/AssetManagement/IAssetManager.h
        src/Core/AssetManagement/AssetImporter.cpp
        src/Core/AssetManagement/AssetImporter.h
        src/Core/AssetManagement/AssetMetadata.h
        src/Core/AssetManagement/TextureImporter.cpp
        src/Core/AssetManagement/TextureImporter.h
        src/Core/AssetManagement/AssetManager.cpp
        src/Core/AssetManagement/AssetManager.h
        src/Core/AssetManagement/EditorAssetManager.cpp
        src/Core/AssetManagement/EditorAssetManager.h
        src/Core/AssetManagement/FontImporter.cpp
        src/Core/AssetManagement/FontImporter.h
        src/Core/AssetManagement/MeshImporter.cpp
        src/Core/AssetManagement/MeshImporter.h
        src/Core/AssetManagement/EngineAssetRegistry.cpp
        src/Core/AssetManagement/EngineAssetRegistry.h
        src/Core/AssetManagement/AssetRegistrySerializer.cpp
        src/Core/AssetManagement/AssetRegistrySerializer.h
        src/Core/TypeSequence.h
        src/Core/Expected.h
        src/Core/String.cpp
        src/Core/String.h
        src/Core/Path.cpp
        src/Core/Path.h
        src/FileSystem/FileStream.cpp
        src/FileSystem/FileStream.h
        src/FileSystem/FileWatcher.cpp
        src/FileSystem/FileWatcher.h
        src/Platform/Windows/WindowsFileWatcher.cpp
        src/Platform/Windows/WindowsFileWatcher.h
        src/Platform/STDFileWatcher.cpp
        src/Platform/STDFileWatcher.h
        src/Platform/MacOS/MacOSFileWatcher.cpp
        src/Platform/MacOS/MacOSFileWatcher.h
        src/Core/ToString.h
        src/Core/Format.cpp
        src/Core/Format.h
        src/Platform/Windows/WindowsUTF8ConsoleOutput.h
        src/Core/Logging/ConsoleOutput.h
        src/Core/Logging/ConsoleOutput.cpp
        src/Renderer/SceneTreeRenderer.cpp
        src/Renderer/SceneTreeRenderer.h
        src/Core/Reflection.cpp
        src/Core/Reflection.h
        src/Scene/Prefab.cpp
        src/Scene/Prefab.h
        src/Core/AssetManagement/PrefabImporter.cpp
        src/Core/AssetManagement/PrefabImporter.h
        src/Gui/ImGui/ImGuiExtension.h
        src/Gui/ImGui/ImGuiExtension.cpp
        src/Core/Hash.cpp
        src/Core/Hash.h
        src/Serialization/ISerializer.h
        src/Serialization/Serializable.h
        src/Serialization/YAMLSerializer.cpp
        src/Serialization/YAMLSerializer.h
        src/Locale/Locale.h
        src/Locale/Locale.cpp
        src/Locale/LocalizationGenerator.cpp
        src/Locale/LocalizationGenerator.h
        src/Locale/ImGuiLocalizationPanel.cpp
        src/Locale/ImGuiLocalizationPanel.h
        src/Core/AssetManagement/LocalizedAsset.cpp
        src/Core/AssetManagement/LocalizedAsset.h
        src/Renderer/SceneRenderer.cpp
        src/Renderer/SceneRenderer.h
        src/Core/Numbers.h
        src/Core/Inlining.h
        src/StackTrace.cpp
        src/StackTrace.h
        src/Utils/Commands.h
        src/Core/Coroutines/Task.h
        src/Core/Coroutines/Awaiters.h
        src/Core/Coroutines/Generator.h
        src/Core/Coroutines/Co_Promise.h
        src/Core/Memory/AllocatorPrimitives.h
        src/Core/Memory/Allocators.h
        src/Hardware.h
        src/JobSystem/JobScheduler.cpp
        src/JobSystem/JobScheduler.h
        src/JobSystem/SpinLock.h
        src/JobSystem/AdaptiveMutex.h
        src/JobSystem/Mutex.h
        src/FileSystem/FileSystem.h
        src/Scripting/NativeToManaged.cpp
        src/Scripting/NativeToManaged.h
        src/Scripting/Runtime.cpp
        src/Platform/Vulkan/VulkanRendererAPI.cpp
        src/Platform/Vulkan/VulkanRendererAPI.h
        src/Platform/Vulkan/VulkanTexture2D.cpp
        src/Platform/Vulkan/VulkanTexture2D.h
        src/Platform/Vulkan/VulkanMesh.cpp
        src/Platform/Vulkan/VulkanMesh.h
        src/Renderer/TopLevelAccelerationStructure.h
        src/Renderer/TopLevelAccelerationStructure.cpp
        src/Platform/Vulkan/VulkanTLAS.cpp
        src/Platform/Vulkan/VulkanTLAS.h
        src/Hardware.cpp
        src/Platform/Vulkan/VulkanShaderModule.cpp
        src/Platform/Vulkan/VulkanShaderModule.h
        src/Platform/Vulkan/VulkanMaterial.cpp
        src/Platform/Vulkan/VulkanMaterial.h
        src/Platform/Vulkan/VulkanInstancedBuffer.cpp
        src/Platform/Vulkan/VulkanInstancedBuffer.h
        src/Platform/Vulkan/VulkanPipeline.cpp
        src/Platform/Vulkan/VulkanPipeline.h
        src/Platform/Vulkan/VulkanFrameBuffer.cpp
        src/Platform/Vulkan/VulkanFrameBuffer.h
        src/Platform/Vulkan/VulkanUniformBuffer.cpp
        src/Platform/Vulkan/VulkanUniformBuffer.h
        src/Platform/Vulkan/VulkanBindingSet.cpp
        src/Platform/Vulkan/VulkanBindingSet.h
        src/JobSystem/InternalJobScheduler.h
        src/JobSystem/InternalJobScheduler.cpp
        src/Windowing/WindowHandler/WinAPIWindowHandler.cpp
        src/Windowing/WindowHandler/WinAPIWindowHandler.h
        src/Platform/Platform.h
        src/Renderer/CommandBuffer.cpp
        src/Platform/Vulkan/VulkanComputePipeline.cpp
        src/Platform/Vulkan/VulkanComputePipeline.h
        src/Core/AssetManagement/MeshSourceImporter.cpp
        src/Core/AssetManagement/MeshSourceImporter.h
        src/Core/AssetManagement/SceneImporter.h
        src/Core/AssetManagement/SceneImporter.cpp
        src/Core/AssetManagement/MeshSource.cpp
        src/Core/AssetManagement/MeshSource.h
        src/Renderer/MaterialData.h
        src/Core/GameConfig.h
        src/Core/GameConfig.cpp
        src/Gui/ImGui/ImGuiOutputConsole.cpp
        src/Gui/ImGui/ImGuiOutputConsole.h
        vendor/SIMDString/SIMDString.h
        vendor/SIMDString/SIMDString.cpp
        src/Gui/MessageBox.h
        src/Gui/MessageBox.cpp
        src/Core/Property.h
        src/Renderer/BufferLayoutSerializer.cpp
        src/Renderer/BufferLayoutSerializer.hpp
)


function(DesktopPlatformsOnly)
    add_compile_definitions(DESKTOP_PLATFORM)
endfunction(DesktopPlatformsOnly)

function(MobilePlatformsOnly)
    add_compile_definitions(MOBILE_PLATFORM)
endfunction(MobilePlatformsOnly)

if(IOS)
    # Covers iOS implementation
    execute_process(COMMAND xcrun --find swiftc
            OUTPUT_VARIABLE _xcrun_out OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE _xcrun_err RESULT_VARIABLE _xcrun_result)
    if(_xcrun_result EQUAL 0 AND EXISTS "${_xcrun_out}")
        set(CMAKE_Swift_COMPILER "${_xcrun_out}")
    else()
        _cmake_find_compiler_path(Swift)
    endif()
    enable_language(CXX C Swift OBJCXX OBJC)
    MobilePlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)

    #list(APPEND SOURCE_FILES "ios/MyClass.mm")
elseif(APPLE)
    # Covers macOS implementation
    enable_language(Swift OBJCXX OBJC)
    set(MACOS_SOURCE_FILES
            src/Platform/MacOS/FileDialogsMacOS.mm
            src/Platform/MacOS/MacOSDragDrop.h
            src/Platform/MacOS/MacOSDragDrop.mm
            src/Platform/MacOS/MacOSThreads.cpp
            src/Platform/MacOS/MacOSPlatform.cpp
            src/Platform/MacOS/MacOSInternal.h
            src/Platform/MacOS/MacOSInternal.mm)
    list(APPEND SOURCE_FILES ${MACOS_SOURCE_FILES})
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
elseif(ANDROID)
    # Covers Android implementation
    MobilePlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
    #list(APPEND SOURCE_FILES "android/MyClass.cpp")
elseif(WIN32)
    # Covers Windows implementation
    set(WINDOWS_SOURCE_FILES
            src/Platform/Windows/WindowsDropSource.cpp
            src/Platform/Windows/WindowsDropSource.h
            src/Platform/Windows/FileDialogsWindows.cpp
            src/Platform/Windows/WindowsString.cpp
            src/Platform/Windows/WindowsString.h
            src/Platform/Windows/WindowsCommands.cpp
            src/Platform/Windows/WindowsThreads.cpp
            src/Platform/Windows/WindowsPlatform.cpp)
    list(APPEND SOURCE_FILES ${WINDOWS_SOURCE_FILES})
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL OFF)
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    set(LINUX_SOURCE_FILES
            src/Platform/Linux/FileDialogsLinux.cpp
            src/Platform/Linux/LinuxThreads.cpp
            src/Platform/Linux/LinuxPlatform.cpp)
    list(APPEND SOURCE_FILES ${LINUX_SOURCE_FILES})
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    set(BEE_COMPILE_VULKAN ON)
    set(BEE_COMPILE_SDL ON)
endif()

#Embedded Resources
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FileEmbed.cmake)
FileEmbedSetup()
if(NOT BEE_DONT_REGENERATE_EMBEDDED_FILES)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Fonts/OpenSans/static/OpenSans-Bold.ttf)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Fonts/OpenSans/static/OpenSans-Regular.ttf)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Fonts/Manrope/static/Manrope-Regular.ttf)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Fonts/Manrope/static/Manrope-Bold.ttf)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/directory.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/file.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/PauseButton.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/PlayButton.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/StopButton.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Textures/SimulateButton.png)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Shaders/Standart2DFragment.glsl)
    FileEmbedAdd(${CMAKE_CURRENT_LIST_DIR}/Assets/Shaders/Standart2DVertex.glsl)
    set(BEE_DONT_REGENERATE_EMBEDDED_FILES ON CACHE BOOL "Don't regenerate embedded files" FORCE)
else ()
    FileEmbedUseGeneratedFiles()
endif ()

set(SCRIPTING_SOURCE_FILES src/Scripting/ScriptingEngine.cpp src/Scripting/ScriptingEngine.h src/Scripting/MAssembly.h src/Scripting/MClass.h src/Scripting/ScriptGlue.cpp src/Scripting/ScriptGlue.h src/Scripting/MObject.h src/Scripting/MMethod.h src/Scripting/GameScript.h src/Scripting/MField.h src/Scripting/MTypes.h src/Scripting/MUtils.cpp src/Scripting/MUtils.h)
list(APPEND SOURCE_FILES ${SCRIPTING_SOURCE_FILES})

add_library(BeeEngine ${SOURCE_FILES})

message(STATUS "Building C# libraries for scripting support")
set(BEE_CSHARP_LIBRARY_PATH ${CMAKE_BINARY_DIR}/src/Engine/net${BEEENGINE_DOTNET_VERSION} CACHE STRING "Path, where built C# libraries are stored")
execute_process(
    COMMAND dotnet build "${CMAKE_CURRENT_LIST_DIR}/../BeeEngineScripting/BeeEngineScripting.sln" -c ${CMAKE_BUILD_TYPE} -o ${BEE_CSHARP_LIBRARY_PATH}
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../BeeEngineScripting"
    RESULT_VARIABLE dotnet_result
    OUTPUT_VARIABLE dotnet_output
    ERROR_VARIABLE dotnet_error
)

if(NOT dotnet_result EQUAL "0")
    message(FATAL_ERROR "Failed to build BeeEngineScripting: ${dotnet_error}")
else()
    message(STATUS "C# libraries for scripting support built successfully")
  message(STATUS "Output ${dotnet_output}")
endif()
#[[if(IOS OR APPLE)
    set(CMAKE_Swift_LANGUAGE_VERSION 5)
    add_library(BeeEngine_Internal_Swift STATIC
            src/Platform/MacOS/SwiftToCpp.h
            #src/Platform/MacOS/FileDialogs.swift
    )
    target_compile_options(BeeEngine_Internal_Swift PUBLIC
            "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>")
    target_include_directories(BeeEngine_Internal_Swift
            PUBLIC src/Platform/MacOS
            )
    target_link_libraries(BeeEngine PRIVATE BeeEngine_Internal_Swift)
endif ()]]

target_link_libraries(BeeEngine PRIVATE file_embed)

install(TARGETS BeeEngine
        EXPORT BeeEngine
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

function(DesktopPlatformsOnlyPostTarget)


endfunction(DesktopPlatformsOnlyPostTarget)

#if(XCODE)
#    set_target_properties(BeeEngine PROPERTIES
#            XCODE_GENERATE_SCHEME ON
#            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
#            XCODE_STD_CXX "c++23")
#endif()

#set(CMAKE_WARN_DEPRECATED FALSE)

#set_property(TARGET BeeEngine PROPERTY COMPILE_WARNING_AS_ERROR ON)

#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

add_subdirectory(vendor/glslang)

if(BEE_COMPILE_WEBGPU)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/InstallWebGPU.cmake)
endif ()

if(IOS)
    # Covers iOS implementation
    target_compile_definitions(BeeEngine PUBLIC IOS)
elseif(APPLE)
    # Covers macOS implementation
    target_compile_definitions(BeeEngine PUBLIC MACOS)
    DesktopPlatformsOnlyPostTarget()
elseif(ANDROID)
    # Covers Android implementation
    target_compile_definitions(BeeEngine PUBLIC ANDROID)
elseif(WIN32)
    # Covers Windows implementation
    target_compile_definitions(BeeEngine PUBLIC WINDOWS)
    DesktopPlatformsOnlyPostTarget()
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    target_compile_definitions(BeeEngine PUBLIC LINUX)
    DesktopPlatformsOnlyPostTarget()
endif()

if(BEE_COMPILE_WEBGPU)
    message("Compiling WebGPU")
    enable_webgpu(BeeEngine)
    target_compile_definitions(BeeEngine PUBLIC BEE_COMPILE_WEBGPU)
endif ()

function(add_sdl_subdirectory)
  add_subdirectory(vendor/sdl)
endfunction()
if (BEE_COMPILE_SDL)
    add_sdl_subdirectory()
    target_compile_definitions(BeeEngine PUBLIC BEE_COMPILE_SDL)
    #target_include_directories(BeeEngine
    #        PUBLIC vendor/sdl/include
    #)
    target_link_libraries(BeeEngine PRIVATE SDL3::SDL3)
endif ()
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/ImGui) # Must be after DesktopPlatformsOnlyPostTarget
add_subdirectory(vendor/gsl)
#add_subdirectory(vendor/BeeAlloc)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/ImGuizmo)
add_subdirectory(vendor/tinyobjloader)
add_subdirectory(vendor/magic_enum)
add_subdirectory(vendor/Box2D)
add_subdirectory(vendor/msdfgen)
add_subdirectory(vendor/KDBindings)
add_subdirectory(vendor/fastgltf)

find_package(ICU REQUIRED COMPONENTS data uc in io i18n dt)
find_package(Boost REQUIRED COMPONENTS context)
find_package(unofficial-nethost CONFIG REQUIRED)

if(BEE_COMPILE_VULKAN)
    find_package(Vulkan REQUIRED)
    target_compile_definitions(BeeEngine PUBLIC BEE_COMPILE_VULKAN)
    target_include_directories(BeeEngine
            PUBLIC vendor/VulkanMemoryAllocator
            )
    target_link_libraries(BeeEngine PUBLIC Vulkan::Vulkan)
endif ()
function(extract_vcpkg_path ICU_LIB_LIST RESULT_VAR)
    list(GET ICU_LIB_LIST 0 ICU_LIB_PATH)
    string(REGEX REPLACE "(.*/vcpkg_installed/[^/]+).*$" "\\1" EXTRACTED_PATH ${ICU_LIB_PATH})
    set(${RESULT_VAR} ${EXTRACTED_PATH} PARENT_SCOPE)
endfunction()

extract_vcpkg_path("${ICU_LIBRARIES}" ICU_VCPKG_PATH)
target_include_directories( ${PROJECT_NAME}
        PUBLIC include
        PUBLIC src
        PUBLIC vendor
        PUBLIC vendor/spdlog/include
        PUBLIC vendor/debugbreak
        PUBLIC vendor/glm/glm
        PUBLIC vendor/stb
        PUBLIC vendor/gsl/include
        #PUBLIC vendor/BeeAlloc/include
        PUBLIC vendor/entt/single_include
        PUBLIC vendor/Incbin
        PUBLIC vendor/yaml-cpp/include
        PUBLIC ${ImGuizmo_SOURCE_DIR}
        PUBLIC vendor/glslang
        PUBLIC vendor/tinyobjloader
        PUBLIC vendor/magic_enum/include

        PUBLIC vendor/utfcpp/include
        PUBLIC ${ICU_VCPKG_PATH}/include
        PUBLIC ${ICU_INCLUDE_DIR}
        )
target_link_libraries( ${PROJECT_NAME}
        #PRIVATE project_warnings
        PUBLIC spdlog::spdlog
        PUBLIC glm
        PUBLIC ImGui
        PUBLIC GSL
        #PUBLIC BeeAlloc
        PUBLIC EnTT
        PUBLIC yaml-cpp
        PUBLIC ImGuizmo
        PUBLIC SPIRV
        PUBLIC glslang
        PUBLIC tinyobjloader
        PUBLIC magic_enum
        PUBLIC Box2D
        PUBLIC MSDFGEN
        PUBLIC ${ICU_LIBRARIES}
        PUBLIC Boost::context
        PRIVATE fastgltf
        PRIVATE unofficial::nethost::nethost
        PUBLIC KDAB::KDBindings
        )

target_compile_definitions(BeeEngine
        PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE
        PUBLIC GLM_FORCE_LEFT_HANDED
        )

if(BEE_ENABLE_RUNTIME_COMPILER)
    add_subdirectory(vendor/RuntimeCompiler)
    target_link_libraries( BeeEngine PUBLIC RuntimeCompiledCPP)
endif ()

# Создаем определение DEBUG только для debug configuration
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

# Включаем проверки только для debug configuration
add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_CHECKS>)

add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_ASSERTS>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_ASSERTS>)

add_compile_definitions($<$<CONFIG:Debug>:BEE_ENABLE_PROFILING>)
add_compile_definitions($<$<CONFIG:Release>:BEE_ENABLE_PROFILING>)

# Включаем валидацию слоев Vulkan только для debug configuration
add_compile_definitions($<$<CONFIG:Debug>:BEE_VULKAN_ENABLE_VALIDATION_LAYERS>)

# Создаем определение RELEASE только для release configuration
add_compile_definitions($<$<CONFIG:Release>:RELEASE>)

# Добавляем имена конфигураций для всех существующих конфигураций сборки
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "Publish")

# Включение Google Sanitizers только для отладочной сборки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Включение Google Sanitizers
    add_compile_options(-fsanitize=address,undefined,leak,memory,thread)

    # Отключение оптимизации компилятора для Google Sanitizers
    add_compile_options(-O0)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Publish")
    add_compile_options(-O3)
endif()

#add_compile_options(-static-libstdc++)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Использование максимального уровня предупреждений
    add_compile_options(-Wall -Wextra -pedantic -Wmost -Wmost-extra)
endif ()

# Добавляем команду сборки, которая будет собирать все конфигурации
add_custom_target(build_all ALL
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all --config $<CONFIG>
        )

target_link_libraries(BeeEngine PRIVATE file_embed)
# for embedding assets
add_compile_definitions(ASSETS_PATH="${PROJECT_SOURCE_DIR}/Assets/")

add_library(BeeEngine::BeeEngine ALIAS BeeEngine)
add_library(BeeEngine::WithScripting ALIAS BeeEngine)
target_compile_definitions(BeeEngine PRIVATE BEE_ENABLE_SCRIPTING)

target_compile_definitions(BeeEngine PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING) # for spdlog

#clang_format(BeeEngine "${SOURCE_FILES}")
