cmake_minimum_required(VERSION 3.24)
project(BeeEngine)
#set(CMAKE_CXX_STANDARD 17)



add_library(BeeEngine STATIC src/BeeEngine.cpp src/Core/TypeDefines.h src/Core/Application.cpp src/Core/Application.h src/Core/EntryPoint.h src/Windowing/WindowHandler/WindowHandler.cpp src/Windowing/WindowHandler/WindowHandler.h src/Windowing/WindowProperties.h src/Windowing/WindowHandler/GlfwWindowHandler.cpp src/Windowing/WindowHandler/GlfwWindowHandler.h src/Core/Logging/Log.h src/Core/Logging/Log.cpp src/Core/Events/Event.h src/Core/Events/EventQueue.cpp src/Core/Events/EventQueue.h src/Core/Layer.h src/Core/LayerStack.cpp src/Core/LayerStack.h src/Core/Input.cpp src/Core/Input.h src/KeyCodes.h src/Iterator.h src/TypeCase.h src/Core/Events/EventImplementations.h src/Windowing/VSync.h src/Renderer/Renderer.cpp src/Renderer/Renderer.h src/Renderer/RendererAPI.h src/Core/EntryPoint.cpp src/Platform/ImGui/ImGuiLoaderOpenGL.h src/Platform/ImGui/ImGuiController.h src/Core/Layer.cpp src/Platform/OpenGL/OpenGLRendererAPI.cpp src/Platform/OpenGL/OpenGLRendererAPI.h src/Renderer/RendererAPI.cpp src/Renderer/GraphicsBuffer.cpp src/Renderer/GraphicsBuffer.h src/Platform/OpenGL/OpenGLVertexBuffer.cpp src/Platform/OpenGL/OpenGLVertexBuffer.h src/Platform/OpenGL/OpenGLIndexBuffer.cpp src/Platform/OpenGL/OpenGLIndexBuffer.h src/Platform/OpenGL/OpenGLVertexArray.cpp src/Platform/OpenGL/OpenGLVertexArray.h src/Renderer/VertexArray.cpp src/Renderer/VertexArray.h src/Renderer/Shader.cpp src/Renderer/Shader.h src/Platform/OpenGL/OpenGLShader.cpp src/Platform/OpenGL/OpenGLShader.h src/Core/Color4.cpp src/Core/Color4.h src/Renderer/BufferLayout.cpp src/Renderer/BufferLayout.h src/Renderer/ShaderLibrary.cpp src/Renderer/ShaderLibrary.h src/Renderer/Texture.cpp src/Renderer/Texture.h src/Platform/OpenGL/OpenGLTexture2D.cpp src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Platform/OpenGL/OpenGLTexture2D.h src/Core/Events/Event.cpp src/Allocator/Allocator.h src/Debug/MemoryProfiler.cpp src/Debug/MemoryProfiler.h src/Debug/DebugLayer.cpp src/Debug/DebugLayer.h src/ObjectPool.cpp src/ObjectPool.h src/Core/SharedPointer.cpp src/Core/SharedPointer.h src/Renderer/FrameBuffer.cpp src/Renderer/FrameBuffer.h src/Platform/OpenGL/OpenGLFrameBuffer.cpp src/Platform/OpenGL/OpenGLFrameBuffer.h src/Core/ResourceManager.cpp src/Core/ResourceManager.h src/Renderer/Renderer2D.cpp src/Renderer/Renderer2D.h src/Core/Cameras/ICamera.cpp src/Core/Cameras/ICamera.h src/Renderer/RectangleProperties.h src/Renderer/Renderer2DAPI.cpp src/Renderer/Renderer2DAPI.h src/Platform/OpenGL/OpenGLRenderer2DAPI.cpp src/Platform/OpenGL/OpenGLRenderer2DAPI.h src/Core/Cameras/OrthographicCamera.cpp src/Core/Cameras/OrthographicCamera.h src/Core/Cameras/OrthographicCameraController.cpp src/Core/Cameras/OrthographicCameraController.h src/Core/Time.cpp src/Core/Time.h src/Debug/OpenGLDebug.h src/Debug/Timer.h src/Debug/Instrumentor.h src/Debug/DebugUtils.h src/Gui/ImGui/FpsCounter.cpp src/Gui/ImGui/FpsCounter.h src/Scene/Scene.cpp src/Scene/Scene.h src/Scene/EntityID.h src/Scene/Components.h src/Scene/Entity.cpp src/Scene/Entity.h src/Core/Cameras/Camera.cpp src/Core/Cameras/Camera.h src/Scene/SceneCamera.cpp src/Scene/SceneCamera.h src/Scene/ScriptableEntity.cpp src/Scene/ScriptableEntity.h src/Platform/ImGui/ImGuiController.cpp vendor/Incbin/incbin.h Assets/EmbeddedResources.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/ImGui)
add_subdirectory(vendor/gsl)
add_subdirectory(vendor/BeeAlloc)
add_subdirectory(vendor/entt)

if(UNIX AND APPLE)
    add_compile_definitions(MACOS)
endif()
if(UNIX AND NOT APPLE)
    add_compile_definitions(LINUX)
endif()
if(WIN32)
    add_compile_definitions(WINDOWS)
endif()


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
        )

target_link_libraries( ${PROJECT_NAME}
        glfw
        glad
        spdlog
        glm
        ImGui
        GSL
        BeeAlloc
        EnTT
        )

# Создаем определение DEBUG только для debug configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# Создаем определение RELEASE только для release configuration
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

# Добавляем имена конфигураций для всех существующих конфигураций сборки
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Добавляем команду сборки, которая будет собирать все конфигурации
add_custom_target(build_all ALL
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all --config $<CONFIG>
        )

add_compile_definitions(ASSETS_PATH="${CMAKE_SOURCE_DIR}/src/Engine/Assets")
