//
// Created by Александр Лебедев on 01.07.2023.
//
#include "ShaderModule.h"
#include "BufferLayoutSerializer.hpp"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "Platform/WebGPU/WebGPUShaderModule.h"
#include "Renderer.h"
#include "Utils/ShaderConverter.h"
#include <filesystem>
#include "Core/Application.h"

#include "Platform/Vulkan/VulkanShaderModule.h"

namespace BeeEngine
{
    Path ShaderModule::s_CachePath = Application::GetInstance().Environment().CacheDirectory() / "Shaders/";
    Ref<ShaderModule> BeeEngine::ShaderModule::Create(const Path& path, ShaderType type)
    {
        if (!std::filesystem::directory_entry(s_CachePath.ToStdPath()).exists())
        {
            std::filesystem::create_directory(s_CachePath.ToStdPath());
        }
        BufferLayout layout;
#if defined(BEE_COMPILE_WEBGPU)
        if (Renderer::GetAPI() == WebGPU)
        {
            std::string wgsl = LoadWGSL(path, type, loadFromCache, layout);
            return CreateRef<Internal::WebGPUShaderModule>(wgsl, type, layout);
        }
#endif
        std::vector<uint32_t> spirv;
        LoadSpirV(path, type, spirv, layout);

        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanShaderModule>(spirv, type, std::move(layout));
#endif
            case NotAvailable:
            default:
                BeeCoreError("Unknown renderer API");
                return nullptr;
        }
    }

    std::vector<uint32_t> ShaderModule::CompileGLSLToSpirVAndCache(const Path& spvPath,
                                                                   const Path& layoutPath,
                                                                   ShaderType type,
                                                                   String& glsl,
                                                                   BufferLayoutBuilder& builder,
                                                                   out<BufferLayout> layout)
    {
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
        if (!ShaderConverter::GLSLtoSPV(shaderType, glsl.data(), result, builder))
        {
            return result;
        }
        layout = ShaderConverter::GenerateLayout(result, builder);
        BeeCoreTrace("Compiled shader to SPIRV");
        try
        {
            File::WriteBinaryFile(spvPath, {(std::byte*)result.data(), result.size() * sizeof(uint32_t)});
            File::WriteFile(layoutPath, BufferLayoutSerializer::Serialize(layout));
        }
        catch (...)
        {
            BeeCoreError("Unable to cache shader {0}", spvPath.GetFileNameWithoutExtension());
        }
        return result;
    }

    std::vector<char> ShaderModule::ReadGLSLShader(const Path& path)
    {
        std::ifstream file(path.ToStdPath());

        if (!file.is_open())
        {
            String errorMsg = "failed to open file: " + path.AsUTF8();
            throw std::runtime_error({errorMsg.begin(), errorMsg.end()});
        }

        size_t fileSize = File::Size(path);
        std::vector<char> buffer(fileSize);

        file.read(buffer.data(), fileSize);
        std::erase(buffer, '\0');

        file.close();
        return buffer;
    }

    std::vector<uint32_t> ShaderModule::LoadSpirVFromCache(const Path& path)
    {
        auto result = File::ReadBinaryFile(path);
        gsl::span<uint32_t> span((uint32_t*)result.data(), result.size() / sizeof(uint32_t));
        BeeCoreTrace("Loaded SPIRV from cache");
        return {span.begin(), span.end()};
    }

    String ShaderModule::CompileSpirVToWGSL(in<std::vector<uint32_t>> spirvCode, in<Path> newPath)
    {
        String wgsl;
#if defined(BEE_COMPILE_WEBGPU)
        bool result = ShaderConverter::SPVtoWGSL(spirvCode, wgsl);
        BeeEnsures(result);
        File::WriteFile(newPath, wgsl);
#endif
        return wgsl;
    }
    String ShaderModule::LoadWGSLFromCache(const Path& path)
    {
        // #if defined(BEE_COMPILE_WEBGPU)
        auto wgsl = ReadGLSLShader(path);
        BeeCoreTrace("Loaded WGSL from cache");
        return String(wgsl.data(), wgsl.size());
        // #endif
    }

    String ShaderModule::LoadWGSL(const Path& path, ShaderType type, bool loadFromCache, out<BufferLayout> layout)
    {
        auto name = path.GetFileNameWithoutExtension().AsUTF8();
        auto newFilepath = s_CachePath + name + GetExtension(type);
        if (path.GetExtension() == (".vert") || path.GetExtension() == (".frag") || path.GetExtension() == (".comp"))
        {
            if (loadFromCache)
            {
                // if(File::Exists(newFilepath))
                //{
                layout = LoadBufferLayoutFromCache(newFilepath + ".layout");
                return LoadWGSLFromCache(newFilepath + ".wgsl");
                //}
            }
            BufferLayoutBuilder builder;
            auto glsl = ReadGLSLShader(path);
            auto glslString = String(glsl.data(), glsl.size());
            ShaderConverter::AnalyzeGLSL(type, builder, glslString);
            auto spirv = CompileGLSLToSpirVAndCache(
                newFilepath + ".spv", newFilepath + ".layout", type, glslString, builder, layout);
            auto wgsl = CompileSpirVToWGSL(spirv, newFilepath + ".wgsl");
            return wgsl;
        }
        /*else if(path.ends_with(".spv"))
        {
            if(loadFromCache)
            {
                if(File::Exists(newFilepath))
                    return LoadWGSLFromCache(newFilepath);
            }
            auto spirv = LoadSpirVFromCache(path);
            if(spirv.empty())
            {
                BeeCoreError("Unable to create shader module");
                return std::string();
            }
            return CompileSpirVToWGSL(spirv, newFilepath);
        }*/
        else
        {
            BeeCoreError("Unknown shader type");
            return String();
        }
    }
    bool ShaderModule::LoadSpirV(const Path& path,
                                 ShaderType type,
                                 out<std::vector<uint32_t>> spirv,
                                 out<BufferLayout> layout)
    {
        // auto name = ResourceManager::GetNameFromFilePath(path);
        // auto newFilepath = s_CachePath + name + GetExtension(type) + ".spv";
        if (path.GetExtension() == (".vert") || path.GetExtension() == (".frag") || path.GetExtension() == (".comp"))
        {
            auto name = path.GetFileNameWithoutExtension().AsUTF8();
            auto newFilepath = s_CachePath + name + GetExtension(type);
            struct CacheData
            {
                Path spv;
                Path layout;
                bool load;
            };
            auto shouldLoadFromCache = [newFilepath]()
            {
                CacheData data{.spv = newFilepath + ".spv", .layout = newFilepath + ".layout", .load = false};
                data.load = File::Exists(data.spv) && File::Exists(data.layout);
                return data; // TODO: add better cache invalidation
            };
            auto cacheData = shouldLoadFromCache();
            if (cacheData.load)
            {
                spirv = LoadSpirVFromCache(cacheData.spv);
                layout = LoadBufferLayoutFromCache(cacheData.layout);
                if (!spirv.empty())
                {
                    return true;
                }
            }
            BufferLayoutBuilder builder;
            auto glsl = ReadGLSLShader(path);
            String glslString(glsl.data(), glsl.size());
            ShaderConverter::AnalyzeGLSL(type, builder, glslString);
            spirv = CompileGLSLToSpirVAndCache(cacheData.spv, cacheData.layout, type, glslString, builder, layout);
        }
        else
        {
            BeeCoreError("Unknown shader type");
            return false;
        }
        if (spirv.empty())
        {
            BeeCoreError("Unable to create shader module");
            return false;
        }
        return true;
    }

    BufferLayout ShaderModule::LoadBufferLayoutFromCache(const Path& path)
    {
        return BufferLayoutSerializer::Deserialize(File::ReadFile(path));
    }
} // namespace BeeEngine
