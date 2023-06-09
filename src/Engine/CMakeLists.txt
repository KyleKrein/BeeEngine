cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
#set(CMAKE_CXX_STANDARD 17)


set(SOURCE_FILES src/BeeEngine.cpp src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Windowing/WindowHandler/GlfwWindowHandler.cpp src/Windowing/WindowHandler/GlfwWindowHandler.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/Renderer/RendererAPI.h src/Core/EntryPoint.cpp src/Platform/ImGui/ImGuiLoaderOpenGL.h src/Platform/ImGui/ImGuiController.h src/Core/Layer.cpp src/Platform/OpenGL/OpenGLRendererAPI.cpp src/Platform/OpenGL/OpenGLRendererAPI.h src/Renderer/RendererAPI.cpp src/Renderer/GraphicsBuffer.cpp src/Renderer/GraphicsBuffer.h src/Platform/OpenGL/OpenGLVertexBuffer.cpp src/Platform/OpenGL/OpenGLVertexBuffer.h src/Platform/OpenGL/OpenGLIndexBuffer.cpp src/Platform/OpenGL/OpenGLIndexBuffer.h src/Platform/OpenGL/OpenGLVertexArray.cpp src/Platform/OpenGL/OpenGLVertexArray.h src/Renderer/VertexArray.cpp src/Renderer/VertexArray.h src/Renderer/Shader.cpp src/Renderer/Shader.h src/Platform/OpenGL/OpenGLShader.cpp src/Platform/OpenGL/OpenGLShader.h src/Core/Color4.cpp src/Core/Color4.h src/Renderer/BufferLayout.cpp src/Renderer/BufferLayout.h src/Renderer/ShaderLibrary.cpp src/Renderer/ShaderLibrary.h src/Renderer/Texture.cpp src/Renderer/Texture.h src/Platform/OpenGL/OpenGLTexture2D.cpp src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Core/Events/Event.cpp src/Allocator/Allocator.h src/Debug/MemoryProfiler.cpp src/Debug/MemoryProfiler.h src/Debug/DebugLayer.cpp src/Debug/DebugLayer.h src/Core/SharedPointer.cpp src/Core/SharedPointer.h src/Renderer/FrameBuffer.cpp src/Renderer/FrameBuffer.h src/Platform/OpenGL/OpenGLFrameBuffer.cpp src/Platform/OpenGL/OpenGLFrameBuffer.h src/Core/ResourceManager.cpp src/Core/ResourceManager.h src/Renderer/Renderer2D.cpp src/Renderer/Renderer2D.h src/Core/Cameras/ICamera.cpp src/Core/Cameras/ICamera.h src/Renderer/RectangleProperties.h src/Renderer/Renderer2DAPI.cpp src/Renderer/Renderer2DAPI.h src/Platform/OpenGL/OpenGLRenderer2DAPI.cpp src/Platform/OpenGL/OpenGLRenderer2DAPI.h src/Core/Cameras/OrthographicCamera.cpp src/Core/Cameras/OrthographicCamera.h src/Core/Cameras/OrthographicCameraController.cpp src/Core/Cameras/OrthographicCameraController.h src/Core/Time.cpp src/Core/Time.h src/Debug/OpenGLDebug.h src/Debug/Timer.h src/Debug/Instrumentor.h src/Core/CodeSafety/Expects.h src/Gui/ImGui/FpsCounter.cpp src/Gui/ImGui/FpsCounter.h src/Scene/Scene.cpp src/Scene/Scene.h src/Scene/EntityID.h src/Scene/Components.h src/Scene/Entity.cpp src/Scene/Entity.h src/Core/Cameras/Camera.cpp src/Core/Cameras/Camera.h src/Scene/SceneCamera.cpp src/Scene/SceneCamera.h src/Scene/ScriptableEntity.cpp src/Scene/ScriptableEntity.h src/Platform/ImGui/ImGuiController.cpp vendor/Incbin/incbin.h Assets/EmbeddedResources.h src/Gui/ImGuiFonts.h src/Scene/SceneSerializer.cpp src/Scene/SceneSerializer.h src/source_location.h src/Property.h src/Utils/FileDialogs.h src/Core/Math/Math.h src/Core/Math/Math.cpp src/Renderer/EditorCamera.cpp src/Renderer/EditorCamera.h Assets/EmbeddedResources.cpp src/Core/CodeSafety/DebugLog.cpp src/Core/CodeSafety/DebugLog.h src/Core/OsPlatform.h src/Renderer/RenderAPI.h src/Renderer/GraphicsDevice.cpp src/Renderer/GraphicsDevice.h src/Renderer/Surface.cpp src/Renderer/Surface.h src/Renderer/CommandPool.cpp src/Renderer/CommandPool.h src/Renderer/DeviceID.cpp src/Renderer/DeviceID.h src/Renderer/GraphicsQueue.cpp src/Renderer/GraphicsQueue.h src/Renderer/SwapChain.cpp src/Renderer/SwapChain.h src/Platform/Vulkan/VulkanGraphicsDevice.cpp src/Platform/Vulkan/VulkanGraphicsDevice.h src/Platform/Vulkan/VulkanCommandPool.cpp src/Platform/Vulkan/VulkanCommandPool.h src/Platform/Vulkan/VulkanGraphicsQueue.cpp src/Platform/Vulkan/VulkanGraphicsQueue.h src/Platform/Vulkan/VulkanSurface.cpp src/Platform/Vulkan/VulkanSurface.h src/Platform/Vulkan/VulkanSwapChain.cpp src/Platform/Vulkan/VulkanSwapChain.h src/Renderer/Instance.cpp src/Renderer/Instance.h src/Platform/Vulkan/VulkanInstance.cpp src/Platform/Vulkan/VulkanInstance.h src/Renderer/QueueFamilyIndices.cpp src/Renderer/QueueFamilyIndices.h)

if(IOS)
    # Covers iOS implementation
    add_compile_definitions(IOS)
    #list(APPEND SOURCE_FILES "ios/MyClass.mm")
elseif(APPLE)
    # Covers macOS implementation
    add_compile_definitions(MACOS)
    list(APPEND SOURCE_FILES "src/Platform/MacOS/FileDialogsMacOS.mm")
elseif(ANDROID)
    # Covers Android implementation
    add_compile_definitions(ANDROID)
    #list(APPEND SOURCE_FILES "android/MyClass.cpp")
elseif(WIN32)
    # Covers Windows implementation
    add_compile_definitions(WINDOWS)
    list(APPEND SOURCE_FILES "src/Platform/Windows/FileDialogsWindows.cpp")
elseif (UNIX AND NOT APPLE)
    # Covers Linux implementation
    add_compile_definitions(LINUX)
    list(APPEND SOURCE_FILES "src/Platform/Linux/FileDialogsLinux.cpp")
endif()

add_library(BeeEngine STATIC ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/ImGui)
add_subdirectory(vendor/gsl)
add_subdirectory(vendor/BeeAlloc)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/yaml-cpp)
add_subdirectory(vendor/ImGuizmo)

find_package(Vulkan REQUIRED)


target_include_directories( ${PROJECT_NAME}
        PUBLIC include
        PUBLIC vendor/glfw/include
        PUBLIC src
        PUBLIC vendor/glad/include
        PUBLIC vendor/spdlog/include
        PUBLIC vendor/debugbreak
        PUBLIC vendor/glm/glm
        PUBLIC vendor/ImGui
        PUBLIC vendor/stb
        PUBLIC vendor/gsl/include
        PUBLIC vendor/BeeAlloc/include
        PUBLIC vendor/entt/single_include
        PUBLIC vendor/Incbin
        PUBLIC vendor/yaml-cpp/include
        PUBLIC vendor/ImGuizmo
        )
target_link_directories( ${PROJECT_NAME}
        PRIVATE src
        PRIVATE vendor/glfw/include
        PRIVATE vendor/glad/include
        PRIVATE include
        PRIVATE vendor/spdlog/include
        PRIVATE vendor/debugbreak
        PRIVATE vendor/glm/glm
        PRIVATE vendor/ImGui
        PRIVATE vendor/stb
        PRIVATE vendor/gsl/include
        PRIVATE vendor/BeeAlloc/include
        PRIVATE vendor/entt/single_include
        PRIVATE vendor/Incbin
        PRIVATE vendor/yaml-cpp
        PRIVATE vendor/ImGuizmo
        )

target_link_libraries( ${PROJECT_NAME}
        PUBLIC glfw
        PUBLIC glad
        PUBLIC spdlog
        PUBLIC glm
        PUBLIC ImGui
        PUBLIC GSL
        PUBLIC BeeAlloc
        PUBLIC EnTT
        PUBLIC yaml-cpp
        PUBLIC ImGuizmo
        PUBLIC Vulkan::Vulkan
        )

# Создаем определение DEBUG только для debug configuration
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Использование максимального уровня предупреждений
    add_compile_options(-Wall -Wextra -pedantic -Wmost -Wmost-extra)
endif ()

# Добавляем команду сборки, которая будет собирать все конфигурации
add_custom_target(build_all ALL
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all --config $<CONFIG>
        )

# for embedding assets
add_compile_definitions(ASSETS_PATH="${CMAKE_SOURCE_DIR}/src/Engine/Assets/")
