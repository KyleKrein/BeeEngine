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
        if(!std::filesystem::directory_entry(s_CachePath).exists())
        {
            std::filesystem::create_directory(s_CachePath);
        }
        BufferLayout layout;
#if defined(BEE_COMPILE_WEBGPU)
        if(Renderer::GetAPI() == WebGPU)
        {
            std::string wgsl = LoadWGSL(path, type, loadFromCache, layout);
            return CreateRef<Internal::WebGPUShaderModule>(wgsl, type, layout);
        }
#endif
        std::vector<uint32_t> spirv;
        LoadSpirV(path, type, loadFromCache, spirv, layout);

        switch (Renderer::GetAPI())
        {
            case Vulkan:
            case OpenGL:
            case Metal:
            case DirectX:
            case WebGPU:
            case NotAvailable:
            default:
                BeeCoreError("Unknown renderer API");
                return nullptr;
        }
    }

    std::vector<uint32_t> ShaderModule::CompileGLSLToSpirV(const String &path, ShaderType type, std::string& glsl, BufferLayoutBuilder& builder, out<BufferLayout> layout)
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
        if (!ShaderConverter::GLSLtoSPV(shaderType, glsl.data(), result))
        {
            return result;
        }
        layout = ShaderConverter::GenerateLayout(result, builder);
        BeeCoreTrace("Compiled shader to SPIRV");
        File::WriteBinaryFile(path, {(std::byte*)result.data(), result.size() * sizeof(uint32_t)});
        //TODO: Cache layout
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
        auto result = File::ReadBinaryFile(path);
        gsl::span<uint32_t> span((uint32_t *)result.data(), result.size() / sizeof(uint32_t));
        BeeCoreTrace("Loaded SPIRV from cache");
        return {span.begin(), span.end()};
    }

    std::string ShaderModule::CompileSpirVToWGSL(in<std::vector<uint32_t>> spirvCode, in<std::string> newPath)
    {
#if defined(BEE_COMPILE_WEBGPU)
        std::string wgsl;
        bool result = ShaderConverter::SPVtoWGSL(spirvCode, wgsl);
        BeeEnsures(result);
        File::WriteFile(newPath, wgsl);
        return wgsl;
#endif
    }
    std::string ShaderModule::LoadWGSLFromCache(const String& path)
    {
#if defined(BEE_COMPILE_WEBGPU)
        auto wgsl = ReadGLSLShader(path);
        BeeCoreTrace("Loaded WGSL from cache");
        return std::string(wgsl.data(), wgsl.size());
#endif
    }

    std::string ShaderModule::LoadWGSL(const String& path, ShaderType type, bool loadFromCache, out<BufferLayout> layout)
    {
        auto name = ResourceManager::GetNameFromFilePath(path);
        auto newFilepath = s_CachePath + name + GetExtension(type);
        if(path.ends_with(".vert") || path.ends_with(".frag") || path.ends_with(".comp"))
        {
            if(loadFromCache)
            {
                //if(File::Exists(newFilepath))
                //{
                    layout = LoadBufferLayoutFromCache(newFilepath + ".layout");
                    return LoadWGSLFromCache(newFilepath + ".wgsl");
                //}
            }
            BufferLayoutBuilder builder;
            auto glsl = ReadGLSLShader(path);
            auto glslString = std::string(glsl.data(), glsl.size());
            ShaderConverter::AnalyzeGLSL(type, builder, glslString);
            auto spirv = CompileGLSLToSpirV(newFilepath + ".spv", type, glslString, builder, layout);
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
            return std::string();
        }
    }
    bool ShaderModule::LoadSpirV(const String& path, ShaderType type, bool loadFromCache, out<std::vector<uint32_t>> spirv, out<BufferLayout> layout)
    {
        //auto name = ResourceManager::GetNameFromFilePath(path);
        //auto newFilepath = s_CachePath + name + GetExtension(type) + ".spv";
        if(path.ends_with(".vert") || path.ends_with(".frag") || path.ends_with(".comp"))
        {
            auto name = ResourceManager::GetNameFromFilePath(path);
            auto newFilepath = s_CachePath + name + GetExtension(type);
            if(loadFromCache)
            {
                spirv = LoadSpirVFromCache(newFilepath + ".spv");
                layout = LoadBufferLayoutFromCache(newFilepath + ".layout");
                if(!spirv.empty())
                {
                    return true;
                }
                return false;
            }
            BufferLayoutBuilder builder;
            auto glsl = ReadGLSLShader(path);
            std::string glslString(glsl.data(), glsl.size());
            ShaderConverter::AnalyzeGLSL(type, builder, glslString);
            spirv = CompileGLSLToSpirV(newFilepath, type, glslString, builder, layout);
        }
        /*else if(path.ends_with(".spv"))
        {
            if(loadFromCache && File::Exists(path))
            {
                spirv = LoadSpirVFromCache(path);
            }
        }*/
        else
        {
            BeeCoreError("Unknown shader type");
            return false;
        }
        if(spirv.empty())
        {
            BeeCoreError("Unable to create shader module");
            return false;
        }
        return true;
    }

    BufferLayout ShaderModule::LoadBufferLayoutFromCache(const BeeEngine::String &path)
    {
        return BufferLayout(); //TODO: load layout
    }
}