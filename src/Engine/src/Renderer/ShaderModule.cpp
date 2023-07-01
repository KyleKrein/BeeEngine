//
// Created by Александр Лебедев on 01.07.2023.
//
#include "ShaderModule.h"
#include "Utils/ShaderConverter.h"
#include "Utils/File.h"
#include "Core/ResourceManager.h"
#include "Renderer.h"
#include "Platform/WebGPU/WebGPUShaderModule.h"
#include <filesystem>

namespace BeeEngine
{
    String ShaderModule::s_CachePath = "Cache/";
    Ref<ShaderModule> BeeEngine::ShaderModule::Create(const String &path, ShaderType type, bool loadFromCache)
    {
        if(std::filesystem::directory_entry(s_CachePath).exists())
        {
            std::filesystem::create_directory(s_CachePath);
        }
        std::vector<uint32_t> spirv;
        if(path.ends_with(".vert") || path.ends_with(".frag") || path.ends_with(".comp"))
        {
            spirv = CompileGLSLToSpirV(path, type, loadFromCache);
        }
        else if(path.ends_with(".spv"))
        {
            spirv = LoadSpirVFromCache(path);
        }
        else
        {
            BeeCoreError("Unknown shader type");
            return nullptr;
        }
        if(spirv.empty())
        {
            BeeCoreError("Unable to create shader module");
            return nullptr;
        }
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUShaderModule>(spirv, type);
                break;
            case Vulkan:
            case OpenGL:
            case Metal:
            case DirectX:
            case NotAvailable:
            default:
                BeeCoreError("Unknown renderer API");
                return nullptr;
        }
    }

    std::vector<uint32_t> ShaderModule::CompileGLSLToSpirV(const String &path, ShaderType type, bool loadFromCache)
    {
        auto name = ResourceManager::GetNameFromFilePath(path);
        constexpr auto GetExtension = [](ShaderType type)
        {
            switch (type)
            {
                case ShaderType::Vertex:
                    return ".vert";
                case ShaderType::Fragment:
                    return ".frag";
                case ShaderType::Compute:
                    return ".comp";
            }
        };
        auto newFilepath = s_CachePath + name + GetExtension(type) + ".spv";
        if(loadFromCache)
        {
            auto result = LoadSpirVFromCache(newFilepath);
            if(!result.empty())
            {
                return result;
            }
        }
        auto file = ReadGLSLShader(path);
        std::vector<uint32_t> result;
        ShaderStage shaderType;
        switch (type)
        {
            case ShaderType::Vertex:
                shaderType = ShaderStage::Vertex;
                break;
            case ShaderType::Fragment:
                shaderType = ShaderStage::Fragment;
                break;
            case ShaderType::Compute:
                shaderType = ShaderStage::Compute;
                break;
        }
        if (!ShaderConverter::GLSLtoSPV(shaderType, file.data(), result))
        {
            return result;
        }
        BeeCoreTrace("Compiled shader to SPIRV");
        File::WriteBinaryFile(newFilepath, {(std::byte*)result.data(), result.size() * sizeof(uint32_t)});
        return result;
    }

    std::vector<char> ShaderModule::ReadGLSLShader(const String &path)
    {
        std::ifstream file(path);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + path);
        }

        size_t fileSize = File::Size(path);
        std::vector<char> buffer(fileSize);

        file.read(buffer.data(), fileSize);
        std::erase(buffer, '\0');

        file.close();
        return buffer;
    }

    std::vector<uint32_t> ShaderModule::LoadSpirVFromCache(const String &path)
    {
        if(File::Exists(path))
        {
            auto result = File::ReadBinaryFile(path);
            gsl::span<uint32_t> span((uint32_t *)result.data(), result.size() / sizeof(uint32_t));
            BeeCoreTrace("Loaded SPIRV from cache");
            return {span.begin(), span.end()};
        }
        else
        {
            BeeCoreTrace("Unable to load SPIRV from cache: File not found");
            return {};
        }
    }
}