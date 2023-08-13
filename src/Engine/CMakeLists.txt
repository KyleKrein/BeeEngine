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

option(BEE_ENABLE_RUNTIME_COMPILER "Enable runtime compiler" ON)

set(SOURCE_FILES src/BeeEngine.cpp src/Core/TypeDefines.h src/Core/EntryPoint.cpp src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/Renderer/RendererAPI.h src/Platform/ImGui/ImGuiLoaderOpenGL.h src/Platform/ImGui/ImGuiController.h src/Core/Layer.cpp src/Platform/OpenGL/OpenGLRendererAPI.cpp src/Platform/OpenGL/OpenGLRendererAPI.h src/Renderer/RendererAPI.cpp src/Renderer/GraphicsBuffer.cpp src/Renderer/GraphicsBuffer.h src/Platform/OpenGL/OpenGLVertexBuffer.cpp src/Platform/OpenGL/OpenGLVertexBuffer.h src/Platform/OpenGL/OpenGLIndexBuffer.cpp src/Platform/OpenGL/OpenGLIndexBuffer.h src/Platform/OpenGL/OpenGLVertexArray.cpp src/Platform/OpenGL/OpenGLVertexArray.h src/Renderer/VertexArray.cpp src/Renderer/VertexArray.h src/Renderer/Shader.cpp src/Renderer/Shader.h src/Platform/OpenGL/OpenGLShader.cpp src/Platform/OpenGL/OpenGLShader.h src/Core/Color4.cpp src/Core/Color4.h src/Renderer/BufferLayout.cpp src/Renderer/BufferLayout.h src/Renderer/Texture.cpp src/Renderer/Texture.h src/Platform/OpenGL/OpenGLTexture2D.cpp src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Core/Events/Event.cpp src/Allocator/Allocator.h src/Debug/MemoryProfiler.cpp src/Debug/MemoryProfiler.h src/Debug/DebugLayer.cpp src/Debug/DebugLayer.h src/Core/SharedPointer.cpp src/Core/SharedPointer.h src/Renderer/FrameBuffer.cpp src/Renderer/FrameBuffer.h src/Platform/OpenGL/OpenGLFrameBuffer.cpp src/Platform/OpenGL/OpenGLFrameBuffer.h src/Core/ResourceManager.cpp src/Core/ResourceManager.h src/Renderer/Renderer2D.cpp src/Renderer/Renderer2D.h src/Core/Cameras/ICamera.cpp src/Core/Cameras/ICamera.h src/Renderer/RectangleProperties.h src/Renderer/Renderer2DAPI.cpp src/Renderer/Renderer2DAPI.h src/Platform/OpenGL/OpenGLRenderer2DAPI.cpp src/Platform/OpenGL/OpenGLRenderer2DAPI.h src/Core/Cameras/OrthographicCamera.cpp src/Core/Cameras/OrthographicCamera.h src/Core/Cameras/OrthographicCameraController.cpp src/Core/Cameras/OrthographicCameraController.h src/Core/Time.cpp src/Core/Time.h src/Debug/OpenGLDebug.h src/Debug/Timer.h src/Debug/Instrumentor.h src/Core/CodeSafety/Expects.h src/Gui/ImGui/FpsCounter.cpp src/Gui/ImGui/FpsCounter.h src/Scene/Scene.cpp src/Scene/Scene.h src/Scene/EntityID.h src/Scene/Components.h src/Scene/Entity.cpp src/Scene/Entity.h src/Core/Cameras/Camera.cpp src/Core/Cameras/Camera.h src/Scene/SceneCamera.cpp src/Scene/SceneCamera.h src/Scene/ScriptableEntity.cpp src/Scene/ScriptableEntity.h src/Platform/ImGui/ImGuiController.cpp vendor/Incbin/incbin.h Assets/EmbeddedResources.h src/Gui/ImGuiFonts.h src/Scene/SceneSerializer.cpp src/Scene/SceneSerializer.h src/source_location.h src/Property.h src/Utils/FileDialogs.h src/Core/Math/Math.h src/Core/Math/Math.cpp src/Renderer/EditorCamera.cpp src/Renderer/EditorCamera.h Assets/EmbeddedResources.cpp src/Core/CodeSafety/DebugLog.cpp src/Core/CodeSafety/DebugLog.h src/Core/OsPlatform.h src/Renderer/RenderAPI.h src/Renderer/GraphicsDevice.cpp src/Renderer/GraphicsDevice.h src/Renderer/Surface.cpp src/Renderer/Surface.h src/Renderer/CommandPool.cpp src/Renderer/CommandPool.h src/Renderer/DeviceID.cpp src/Renderer/DeviceID.h src/Renderer/GraphicsQueue.cpp src/Renderer/GraphicsQueue.h src/Renderer/SwapChain.cpp src/Renderer/SwapChain.h src/Platform/Vulkan/VulkanGraphicsDevice.cpp src/Platform/Vulkan/VulkanGraphicsDevice.h src/Platform/Vulkan/VulkanCommandPool.cpp src/Platform/Vulkan/VulkanCommandPool.h src/Platform/Vulkan/VulkanGraphicsQueue.cpp src/Platform/Vulkan/VulkanGraphicsQueue.h src/Platform/Vulkan/VulkanSurface.cpp src/Platform/Vulkan/VulkanSurface.h src/Platform/Vulkan/VulkanSwapChain.cpp src/Platform/Vulkan/VulkanSwapChain.h src/Renderer/Instance.cpp src/Renderer/Instance.h src/Platform/Vulkan/VulkanInstance.cpp src/Platform/Vulkan/VulkanInstance.h src/Renderer/QueueFamilyIndices.cpp src/Renderer/QueueFamilyIndices.h src/Utils/File.cpp src/Utils/File.h src/Platform/Vulkan/VulkanShaderModule.cpp src/Platform/Vulkan/VulkanShaderModule.h src/Platform/Vulkan/VulkanPipeline.cpp src/Platform/Vulkan/VulkanPipeline.h src/Platform/Vulkan/VulkanRenderPass.cpp src/Platform/Vulkan/VulkanRenderPass.h src/Platform/Vulkan/VulkanFramebuffer.cpp src/Platform/Vulkan/VulkanFramebuffer.h src/Platform/Vulkan/VulkanCommandBuffer.cpp src/Platform/Vulkan/VulkanCommandBuffer.h src/Platform/Vulkan/VulkanSemaphore.cpp src/Platform/Vulkan/VulkanSemaphore.h src/Platform/Vulkan/VulkanFence.cpp src/Platform/Vulkan/VulkanFence.h src/Platform/ImGui/ImGuiControllerVulkan.h src/Platform/ImGui/ImGuiControllerVulkan.cpp src/Utils/ShaderConverter.h src/Utils/ShaderConverter.cpp src/Platform/Vulkan/VulkanModel.h src/Platform/Vulkan/VulkanModel.cpp vendor/VulkanMemoryAllocator/vk_mem_alloc.h src/Core/CodeSafety/NotNull.h src/Core/CodeSafety/BoundsChecking.h src/Platform/Vulkan/VulkanRendererAPI.cpp src/Platform/Vulkan/VulkanRendererAPI.h src/Platform/Vulkan/VulkanBuffer.h src/Renderer/Vertex.h src/Core/DeletionQueue.cpp src/Core/DeletionQueue.h src/Utils/ModelLoader.cpp src/Utils/ModelLoader.h src/Platform/Vulkan/VulkanImage.h src/Platform/Vulkan/Utils.h src/Platform/Vulkan/Utils.cpp src/Renderer/AssetManager.cpp src/Renderer/AssetManager.h src/Renderer/Mesh.h src/Renderer/Mesh.cpp src/Renderer/Material.cpp src/Renderer/Material.h src/Platform/Vulkan/VulkanMaterial.cpp src/Platform/Vulkan/VulkanMaterial.h src/Platform/Vulkan/VulkanRenderer2DAPI.cpp src/Platform/Vulkan/VulkanRenderer2DAPI.h src/Windowing/WindowHandler/SDLWindowHandler.cpp src/Windowing/WindowHandler/SDLWindowHandler.h src/Platform/Vulkan/VulkanImage.cpp src/Platform/WebGPU/WebGPUInstance.cpp src/Platform/WebGPU/WebGPUInstance.h src/Platform/WebGPU/WebGPUGraphicsDevice.cpp src/Platform/WebGPU/WebGPUGraphicsDevice.h src/Platform/WebGPU/WebGPUSwapchain.cpp src/Platform/WebGPU/WebGPUSwapchain.h src/Platform/WebGPU/WebGPURendererAPI.cpp src/Platform/WebGPU/WebGPURendererAPI.h src/Renderer/CommandBuffer.h src/Platform/WebGPU/WebGPUCommandBuffer.cpp src/Platform/WebGPU/WebGPUCommandBuffer.h src/Platform/ImGui/ImGuiControllerWebGPU.h src/Platform/ImGui/ImGuiControllerWebGPU.cpp src/Renderer/RenderPass.h src/Platform/WebGPU/WebGPUPipeline.cpp src/Platform/WebGPU/WebGPUPipeline.h src/Renderer/Pipeline.h src/Renderer/ShaderTypes.h src/Renderer/ShaderModule.h src/Renderer/ShaderModule.cpp src/Platform/WebGPU/WebGPUShaderModule.cpp src/Platform/WebGPU/WebGPUShaderModule.h src/Renderer/Pipeline.cpp src/Platform/WebGPU/WebGPUMesh.cpp src/Platform/WebGPU/WebGPUMesh.h src/Renderer/UniformBuffer.cpp src/Renderer/UniformBuffer.h src/Platform/WebGPU/WebGPUUniformBuffer.cpp src/Platform/WebGPU/WebGPUUniformBuffer.h src/Renderer/InstancedBuffer.h src/Platform/WebGPU/WebGPUInstancedBuffer.cpp src/Platform/WebGPU/WebGPUInstancedBuffer.h src/Core/RestartApplication.h src/Renderer/Model.h src/Platform/WebGPU/WebGPUMaterial.cpp src/Platform/WebGPU/WebGPUMaterial.h src/Renderer/Model.cpp src/Platform/WebGPU/WebGPUModel.cpp src/Platform/WebGPU/WebGPUTexture2D.cpp src/Platform/WebGPU/WebGPUTexture2D.h vendor/Implementations.cpp src/Renderer/BindingSet.cpp src/Renderer/BindingSet.h src/Renderer/IBindable.h src/Renderer/MaterialDescriptor.h src/Core/ValueType.h src/Platform/WebGPU/WebGPUBindingSet.cpp src/Platform/WebGPU/WebGPUBindingSet.h src/Platform/WebGPU/WebGPUBufferPool.cpp src/Platform/WebGPU/WebGPUBufferPool.h src/Renderer/RenderingQueue.cpp src/Renderer/RenderingQueue.h src/Renderer/InstancedBuffer.cpp src/Renderer/RendererStatistics.h src/Gui/ImGui/RendererStatisticsGUI.cpp src/Gui/ImGui/RendererStatisticsGUI.h src/Platform/WebGPU/WebGPUFramebuffer.cpp src/Platform/WebGPU/WebGPUFramebuffer.h src/Core/FramePtr.cpp src/Core/FramePtr.h src/Scene/INativeScriptRegistry.h src/Scene/NativeScriptFactory.cpp src/Scene/NativeScriptFactory.h src/Scene/INativeScriptFactory.h src/Utils/DynamicLibrary.cpp src/Utils/DynamicLibrary.h src/Core/Logging/GameLogger.cpp src/Core/Logging/GameLogger.h src/Scene/DefaultNativeScript.h src/Core/UUID.cpp src/Core/UUID.h src/Scripting/ScriptingEngine.cpp src/Scripting/ScriptingEngine.h src/Scripting/MAssembly.h src/Scripting/MonoRuntime.cpp src/Scripting/MClass.h src/Scripting/ScriptGlue.cpp src/Scripting/ScriptGlue.h src/Scripting/MObject.h src/Scripting/MMethod.h src/Scripting/GameScript.h src/Scripting/MField.h src/Scripting/MTypes.h src/Scripting/MUtils.cpp src/Scripting/MUtils.h src/Scene/Components.cpp src/Allocator/AllocatorStatistics.h src/Allocator/AllocatorStatistics.h
        src/Renderer/Font.cpp
        src/Renderer/Font.h
        src/Renderer/MSDFData.h
        src/Renderer/TextRenderingConfiguration.h)

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
    set(BEE_COMPILE_WEBGPU ON)

    #list(APPEND SOURCE_FILES "ios/MyClass.mm")
elseif(APPLE)
    # Covers macOS implementation
    enable_language(Swift OBJCXX OBJC)
    list(APPEND SOURCE_FILES src/Platform/MacOS/FileDialogsMacOS.mm)
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU ON)
elseif(ANDROID)
    # Covers Android implementation
    MobilePlatformsOnly()
    set(BEE_COMPILE_WEBGPU OFF)
    #list(APPEND SOURCE_FILES "android/MyClass.cpp")
elseif(WIN32)
    # Covers Windows implementation
    list(APPEND SOURCE_FILES "src/Platform/Windows/FileDialogsWindows.cpp")
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU ON)
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    list(APPEND SOURCE_FILES "src/Platform/Linux/FileDialogsLinux.cpp")
    DesktopPlatformsOnly()
    set(BEE_COMPILE_WEBGPU ON)
endif()

#Embedded Resources
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FileEmbed.cmake)
FileEmbedSetup()
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

include(${CMAKE_CURRENT_LIST_DIR}/cmake/LinkMono.cmake)

add_library(BeeEngine ${SOURCE_FILES})

set(MONO_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/vendor/mono/libs/)
if(PROJECT_IS_TOP_LEVEL)
    configureMono(${MONO_DIRECTORY})
endif ()
enable_mono(BeeEngine)
target_link_libraries(BeeEngine PRIVATE file_embed)

install(TARGETS BeeEngine
        EXPORT BeeEngine
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

function(DesktopPlatformsOnlyPostTarget)
    if(BEE_COMPILE_WEBGPU)
        message("Compiling WebGPU")
        enable_webgpu(BeeEngine)
        target_link_libraries( ${PROJECT_NAME} PUBLIC libtint)
        target_compile_definitions(BeeEngine PUBLIC BEE_COMPILE_WEBGPU)
    endif()

endfunction(DesktopPlatformsOnlyPostTarget)

#if(XCODE)
#    set_target_properties(BeeEngine PROPERTIES
#            XCODE_GENERATE_SCHEME ON
#            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
#            XCODE_STD_CXX "c++23")
#endif()

#set(CMAKE_WARN_DEPRECATED FALSE)

#set_property(TARGET BeeEngine PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

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
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/ImGui) # Must be after DesktopPlatformsOnlyPostTarget
add_subdirectory(vendor/gsl)
#add_subdirectory(vendor/BeeAlloc)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/ImGuizmo)
add_subdirectory(vendor/tinyobjloader)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/magic_enum)
add_subdirectory(vendor/Box2D)
add_subdirectory(vendor/msdfgen)

find_package(Vulkan REQUIRED)
target_include_directories( ${PROJECT_NAME}
        PUBLIC include
        PUBLIC src
        PUBLIC vendor/glad/include
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
        PUBLIC vendor/VulkanMemoryAllocator
        PUBLIC vendor/tinyobjloader
        PUBLIC vendor/sdl/include
        PUBLIC vendor/magic_enum/include
        )
target_link_libraries( ${PROJECT_NAME}
        #PRIVATE project_warnings
        PUBLIC glad
        PUBLIC spdlog::spdlog
        PUBLIC glm
        PUBLIC ImGui
        PUBLIC GSL
        #PUBLIC BeeAlloc
        PUBLIC EnTT
        PUBLIC yaml-cpp
        PUBLIC ImGuizmo
        PUBLIC Vulkan::Vulkan
        PUBLIC SPIRV
        PUBLIC glslang
        PUBLIC tinyobjloader
        PUBLIC SDL3::SDL3
        PUBLIC magic_enum
        PUBLIC Box2D
        PUBLIC MSDFGEN
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

add_compile_options(-static-libstdc++)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Использование максимального уровня предупреждений
    add_compile_options(-Wall -Wextra -pedantic -Wmost -Wmost-extra)
endif ()

# Добавляем команду сборки, которая будет собирать все конфигурации
add_custom_target(build_all ALL
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all --config $<CONFIG>
        )

# for embedding assets
add_compile_definitions(ASSETS_PATH="${PROJECT_SOURCE_DIR}/Assets/")
