#include "ShaderConverter.h"
#include "Core/Logging/Log.h"
#include "Core/Numbers.h"
#include "SPIRV/GlslangToSpv.h"
#include "glslang/Include/BaseTypes.h"
#include "glslang/Public/ShaderLang.h"
#include <cstdint>
// #include <spirv_cross/spirv_cross.hpp>
#if defined(BEE_COMPILE_WEBGPU)
#include "src/tint/writer/wgsl/generator_impl.h"
#else
// #include <src/tint/lang/spirv/reader/ast_parser/parse.h>
#endif
#include "Renderer/ShaderTypes.h"
#if defined(BEE_TINT)
#include <tint/tint.h>
#endif

#include "FileSystem/File.h"

namespace BeeEngine
{
    class CustomIncluder final : public glslang::TShader::Includer
    {
    public:
        IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth) override
        {
            String code = File::ReadFile(headerName);
            char* result = new char[strlen(code.c_str())];
            strcpy(result, code.c_str());
            return new IncludeResult(headerName, result, code.size(), result);
        }

        IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t) override
        {
            String code = File::ReadFile(std::filesystem::current_path() / "Shaders" / headerName);
            size_t length = strlen(code.c_str()) + 1;
            char* result = new char[length];
            strcpy(result, code.c_str());
            return new IncludeResult(headerName, result, length - 1, result);
        }

        void releaseInclude(IncludeResult* result) override
        {
            delete[] result->headerData;
            delete result;
        }

        ~CustomIncluder() override = default;
    };

    BeeEngine::ShaderUniformDataType GlslangToShaderUniformDataType(const glslang::TType& type)
    {
        auto basicType = type.getBasicType();
        if (basicType == glslang::EbtSampler)
        {
            auto& sampler = type.getSampler();
            if (sampler.isTexture())
            {
                return BeeEngine::ShaderUniformDataType::SampledTexture;
            }
            return BeeEngine::ShaderUniformDataType::Sampler;
        }

        return ShaderUniformDataType::Data;
    }
    size_t GetUniformSize(const glslang::TType& type)
    {
        if (type.getBasicType() == glslang::EbtBlock)
        {
            size_t totalSize = 0;
            for (const auto& member : *type.getStruct())
            {
                totalSize += GetUniformSize(*member.type);
            }
            return totalSize;
        }
        // Размеры для базовых типов
        size_t baseSize = 0;
        switch (type.getBasicType())
        {
            case glslang::EbtFloat:
                baseSize = sizeof(float32_t);
                break;
            case glslang::EbtInt:
                baseSize = sizeof(int32_t);
                break;
            case glslang::EbtUint:
                baseSize = sizeof(uint32_t);
                break;
            case glslang::EbtBool:
                baseSize = sizeof(bool);
                break;
            case glslang::EbtFloat16:
                baseSize = sizeof(float32_t) / 2; // Для half (если поддерживается)
            default:
                return 0; // Неизвестный тип
        }

        // Обработка векторных типов
        if (type.isVector())
        {
            return baseSize * type.getVectorSize(); // Вектор
        }

        // Обработка матриц
        if (type.isMatrix())
        {
            return baseSize * type.getMatrixCols() * type.getMatrixRows(); // Матрица
        }

        // Обработка структур
        if (type.isStruct())
        {
            size_t totalSize = 0;
            for (const auto& member : *type.getStruct())
            {
                totalSize += GetUniformSize(*member.type);
            }
            return totalSize; // Возвращаем общий размер структуры
        }

        return baseSize; // Для скалярных типов
    }
    ShaderDataType GlslangToShaderDataType(const glslang::TType& type)
    {
        // Проверка базового типа
        switch (type.getBasicType())
        {
            case glslang::EbtFloat:
                switch (type.getVectorSize())
                {
                    case 1:
                        return ShaderDataType::Float; // float
                    case 2:
                        return ShaderDataType::Float2; // vec2
                    case 3:
                        return ShaderDataType::Float3; // vec3
                    case 4:
                        return ShaderDataType::Float4; // vec4
                    default:
                        // return ShaderDataType::NoneData; // Неверный размер
                }
                if (type.getMatrixCols() == 4 && type.getMatrixRows() == 4)
                {
                    return ShaderDataType::Mat4;
                }
                if (type.getMatrixCols() == 3 && type.getMatrixRows() == 3)
                {
                    return ShaderDataType::Mat3;
                }
                break;
            case glslang::EbtInt:
                switch (type.getVectorSize())
                {
                    case 1:
                        return ShaderDataType::Int; // int
                    case 2:
                        return ShaderDataType::Int2; // ivec2
                    case 3:
                        return ShaderDataType::Int3; // ivec3
                    case 4:
                        return ShaderDataType::Int4; // ivec4
                    default:
                        // return ShaderDataType::NoneData; // Неверный размер
                }
                break;
            case glslang::EbtUint:
                switch (type.getVectorSize())
                {
                    case 1:
                        return ShaderDataType::UInt; // uint
                    case 2:
                        return ShaderDataType::UInt2; // uvec2
                    case 3:
                        return ShaderDataType::UInt3; // uvec3
                    case 4:
                        return ShaderDataType::UInt4; // uvec4
                    default:
                        // return ShaderDataType::NoneData; // Неверный размер
                }
                break;
            case glslang::EbtBool:
                return ShaderDataType::Bool; // bool

            case glslang::EbtFloat16:
                return ShaderDataType::Half; // half (если поддерживается)

            case glslang::EbtStruct:
                // Для структур можно добавить дополнительные проверки
                // в зависимости от их использования
                // return ShaderDataType::NoneData; // Для простоты

                // Если нужно обрабатывать матрицы
                // case glslang::Ebt:
                // return ShaderDataType::Mat3; // mat3
                // case glslang::EbtMat4:
                // return ShaderDataType::Mat4; // mat4

            default:
                break; // return ShaderDataType::NoneData; // Неверный тип
        }
        BeeCoreTrace("Unknown type: {0}", type.getBasicType());
        return ShaderDataType::NoneData;
    }

    void glslang_reflection(glslang::TIntermediate* intermediate, BeeEngine::BufferLayoutBuilder& layout)
    {
        class ReflectionTraverser : public glslang::TIntermTraverser
        {
        public:
            ReflectionTraverser(BeeEngine::BufferLayoutBuilder& layout) : layout(layout) {}

            void visitSymbol(glslang::TIntermSymbol* symbol) override
            {
                auto qualifier = symbol->getQualifier();
                auto name = String{symbol->getName().c_str()};
                if (name.starts_with("gl_"))
                {
                    return;
                }

                // Входные переменные
                if (qualifier.isPipeInput())
                {
                    auto type = GlslangToShaderDataType(symbol->getType());
                    if (type != ShaderDataType::NoneData)
                    {
                        layout.AddInput(type, name, qualifier.layoutLocation);
                    }
                }
                // Выходные переменные
                else if (qualifier.isPipeOutput())
                {
                    auto type = GlslangToShaderDataType(symbol->getType());
                    if (type != ShaderDataType::NoneData)
                    {
                        layout.AddOutput(type, name, qualifier.layoutLocation);
                    }
                }
                // Униформы
                else if (qualifier.isUniformOrBuffer())
                {
                    if (qualifier.isUniform())
                    {
                        layout.AddUniform(GlslangToShaderUniformDataType(symbol->getType()),
                                          qualifier.layoutSet,
                                          qualifier.layoutBinding,
                                          GetUniformSize(symbol->getType()));
                    }
                }
            }

        private:
            BufferLayoutBuilder& layout;
        };

        ReflectionTraverser traverser{layout};

        auto root = intermediate->getTreeRoot();
        root->traverse(&traverser);
    }

    bool ShaderConverter::GLSLtoSPV(const ShaderStage shader_type,
                                    const char* pshader,
                                    std::vector<uint32_t>& spirv,
                                    BufferLayoutBuilder& layout)
    {
        EShLanguage stage = static_cast<EShLanguage>(shader_type);
        glslang::TShader shader(stage);
        glslang::TProgram program;
        const char* shaderStrings[1];
        TBuiltInResource Resources = {};
        InitResources(Resources);

        // Enable SPIR-V and Vulkan rules when parsing GLSL
        EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

        shaderStrings[0] = pshader;
        shader.setStrings(shaderStrings, 1);
        shader.setAutoMapBindings(true);
        shader.setAutoMapLocations(true);
        CustomIncluder includer;
        if (!shader.parse(&Resources, 460, false, messages, includer))
        {
            BeeCoreError(shader.getInfoLog());
            BeeCoreError(shader.getInfoDebugLog());
            return false; // something didn't work
        }

        program.addShader(&shader);
        //
        // Program-level processing...
        //

        if (!program.link(messages))
        {
            BeeCoreError(shader.getInfoLog());
            BeeCoreError(shader.getInfoDebugLog());
            Finalize();
            return false;
        }
        glslang_reflection(program.getIntermediate(stage), layout);
        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
        return true;
    }

    void ShaderConverter::InitResources(TBuiltInResource& Resources)
    {
        Resources.maxLights = 32;
        Resources.maxClipPlanes = 6;
        Resources.maxTextureUnits = 32;
        Resources.maxTextureCoords = 32;
        Resources.maxVertexAttribs = 64;
        Resources.maxVertexUniformComponents = 4096;
        Resources.maxVaryingFloats = 64;
        Resources.maxVertexTextureImageUnits = 32;
        Resources.maxCombinedTextureImageUnits = 80;
        Resources.maxTextureImageUnits = 32;
        Resources.maxFragmentUniformComponents = 4096;
        Resources.maxDrawBuffers = 32;
        Resources.maxVertexUniformVectors = 128;
        Resources.maxVaryingVectors = 8;
        Resources.maxFragmentUniformVectors = 16;
        Resources.maxVertexOutputVectors = 16;
        Resources.maxFragmentInputVectors = 15;
        Resources.minProgramTexelOffset = -8;
        Resources.maxProgramTexelOffset = 7;
        Resources.maxClipDistances = 8;
        Resources.maxComputeWorkGroupCountX = 65535;
        Resources.maxComputeWorkGroupCountY = 65535;
        Resources.maxComputeWorkGroupCountZ = 65535;
        Resources.maxComputeWorkGroupSizeX = 1024;
        Resources.maxComputeWorkGroupSizeY = 1024;
        Resources.maxComputeWorkGroupSizeZ = 64;
        Resources.maxComputeUniformComponents = 1024;
        Resources.maxComputeTextureImageUnits = 16;
        Resources.maxComputeImageUniforms = 8;
        Resources.maxComputeAtomicCounters = 8;
        Resources.maxComputeAtomicCounterBuffers = 1;
        Resources.maxVaryingComponents = 60;
        Resources.maxVertexOutputComponents = 64;
        Resources.maxGeometryInputComponents = 64;
        Resources.maxGeometryOutputComponents = 128;
        Resources.maxFragmentInputComponents = 128;
        Resources.maxImageUnits = 8;
        Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
        Resources.maxCombinedShaderOutputResources = 8;
        Resources.maxImageSamples = 0;
        Resources.maxVertexImageUniforms = 0;
        Resources.maxTessControlImageUniforms = 0;
        Resources.maxTessEvaluationImageUniforms = 0;
        Resources.maxGeometryImageUniforms = 0;
        Resources.maxFragmentImageUniforms = 8;
        Resources.maxCombinedImageUniforms = 8;
        Resources.maxGeometryTextureImageUnits = 16;
        Resources.maxGeometryOutputVertices = 256;
        Resources.maxGeometryTotalOutputComponents = 1024;
        Resources.maxGeometryUniformComponents = 1024;
        Resources.maxGeometryVaryingComponents = 64;
        Resources.maxTessControlInputComponents = 128;
        Resources.maxTessControlOutputComponents = 128;
        Resources.maxTessControlTextureImageUnits = 16;
        Resources.maxTessControlUniformComponents = 1024;
        Resources.maxTessControlTotalOutputComponents = 4096;
        Resources.maxTessEvaluationInputComponents = 128;
        Resources.maxTessEvaluationOutputComponents = 128;
        Resources.maxTessEvaluationTextureImageUnits = 16;
        Resources.maxTessEvaluationUniformComponents = 1024;
        Resources.maxTessPatchComponents = 120;
        Resources.maxPatchVertices = 32;
        Resources.maxTessGenLevel = 64;
        Resources.maxViewports = 16;
        Resources.maxVertexAtomicCounters = 0;
        Resources.maxTessControlAtomicCounters = 0;
        Resources.maxTessEvaluationAtomicCounters = 0;
        Resources.maxGeometryAtomicCounters = 0;
        Resources.maxFragmentAtomicCounters = 8;
        Resources.maxCombinedAtomicCounters = 8;
        Resources.maxAtomicCounterBindings = 1;
        Resources.maxVertexAtomicCounterBuffers = 0;
        Resources.maxTessControlAtomicCounterBuffers = 0;
        Resources.maxTessEvaluationAtomicCounterBuffers = 0;
        Resources.maxGeometryAtomicCounterBuffers = 0;
        Resources.maxFragmentAtomicCounterBuffers = 1;
        Resources.maxCombinedAtomicCounterBuffers = 1;
        Resources.maxAtomicCounterBufferSize = 16384;
        Resources.maxTransformFeedbackBuffers = 4;
        Resources.maxTransformFeedbackInterleavedComponents = 64;
        Resources.maxCullDistances = 8;
        Resources.maxCombinedClipAndCullDistances = 8;
        Resources.maxSamples = 4;
        Resources.maxMeshOutputVerticesNV = 256;
        Resources.maxMeshOutputPrimitivesNV = 512;
        Resources.maxMeshWorkGroupSizeX_NV = 32;
        Resources.maxMeshWorkGroupSizeY_NV = 1;
        Resources.maxMeshWorkGroupSizeZ_NV = 1;
        Resources.maxTaskWorkGroupSizeX_NV = 32;
        Resources.maxTaskWorkGroupSizeY_NV = 1;
        Resources.maxTaskWorkGroupSizeZ_NV = 1;
        Resources.maxMeshViewCountNV = 4;
        Resources.limits.nonInductiveForLoops = 1;
        Resources.limits.whileLoops = 1;
        Resources.limits.doWhileLoops = 1;
        Resources.limits.generalUniformIndexing = 1;
        Resources.limits.generalAttributeMatrixVectorIndexing = 1;
        Resources.limits.generalVaryingIndexing = 1;
        Resources.limits.generalSamplerIndexing = 1;
        Resources.limits.generalVariableIndexing = 1;
        Resources.limits.generalConstantMatrixVectorIndexing = 1;
    }

    void ShaderConverter::Init()
    {
        glslang::InitializeProcess();
    }

    void ShaderConverter::Finalize()
    {
        glslang::FinalizeProcess();
    }

    bool ShaderConverter::SPVtoWGSL(const std::vector<uint32_t>& spirv, String& wgsl)
    {
#if !defined(BEE_COMPILE_WEBGPU)
        BeeCoreError("WGSL is only supported on WebGPU");
        return false;
#else
        try
        {
            auto program = tint::reader::spirv::Parse(spirv);
            tint::writer::wgsl::GeneratorImpl generator(&program);
            generator.Generate();
            wgsl = generator.Result();
        }
        catch (const std::exception& e)
        {
            BeeCoreError("Failed to convert SPIR-V to WGSL: {}", e.what());
            return false;
        }
        BeeCoreTrace("WGSL compiled successfully");
        return true;
#endif
    }
#if defined(BEE_TINT)
    static ShaderDataType GetShaderDataType(tint::inspector::ComponentType component,
                                            tint::inspector::CompositionType composition)
    {
        using namespace tint::inspector;
        switch (composition)
        {
            case CompositionType::kScalar:
                switch (component)
                {
                    case ComponentType::kF32:
                        return ShaderDataType::Float;
                    case ComponentType::kU32:
                        return ShaderDataType::UInt;
                    case ComponentType::kI32:
                        return ShaderDataType::Int;
                    case ComponentType::kF16:
                        return ShaderDataType::Half;
                    default:
                        break;
                }
                break;
            case CompositionType::kVec2:
                if (component == ComponentType::kF32)
                    return ShaderDataType::Float2;
                break;
            case CompositionType::kVec3:
                if (component == ComponentType::kF32)
                    return ShaderDataType::Float3;
                break;
            case CompositionType::kVec4:
                if (component == ComponentType::kF32)
                    return ShaderDataType::Float4;
                break;
        }
        BeeCoreError("Unknown shader data type");
        return ShaderDataType::NoneData;
    }

    static ShaderUniformDataType GetShaderUniformDataType(tint::inspector::ResourceBinding::ResourceType resourceType)
    {
        switch (resourceType)
        {
            case tint::inspector::ResourceBinding::ResourceType::kUniformBuffer:
                return ShaderUniformDataType::Data;
            case tint::inspector::ResourceBinding::ResourceType::kStorageBuffer:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kReadOnlyStorageBuffer:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kSampler:
                return ShaderUniformDataType::Sampler;
            case tint::inspector::ResourceBinding::ResourceType::kComparisonSampler:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kSampledTexture:
                return ShaderUniformDataType::SampledTexture;
            case tint::inspector::ResourceBinding::ResourceType::kMultisampledTexture:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kWriteOnlyStorageTexture:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kDepthTexture:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kDepthMultisampledTexture:
                break;
            case tint::inspector::ResourceBinding::ResourceType::kExternalTexture:
                break;
        }
        return ShaderUniformDataType::Unknown;
    }
#endif
#if defined(BEE_TINT) or defined(BEE_COMPILE_WEBGPU)
    BufferLayout ShaderConverter::GenerateLayout(in<std::vector<uint32_t>> spirv, BufferLayoutBuilder& builder)
    {
        BeeCoreTrace("Generating buffer layout for spirv shader");
#if defined(BEE_COMPILE_WEBGPU)
        auto shader = tint::reader::spirv::Parse(spirv);
        tint::inspector::Inspector inspector(&shader);
#else
        tint::spirv::reader::Options options;
        options.allowed_features = tint::wgsl::AllowedFeatures::Everything();
        auto shader = tint::spirv::reader::Read(spirv, options);
        tint::inspector::Inspector inspector(shader);
#endif
        auto entryPoints = inspector.GetEntryPoints();
        for (auto& entryPoint : entryPoints)
        {
            if (entryPoint.name != "main") // support for only main entry point
                continue;
            auto& inputs = entryPoint.input_variables;
            builder.NumberOfInputs(inputs.size());
            for (auto& input : inputs)
            {
#if defined(BEE_COMPILE_WEBGPU)
                builder.AddInput(GetShaderDataType(input.component_type, input.composition_type),
                                 input.name,
                                 input.location_attribute);
#else
                builder.AddInput(GetShaderDataType(input.component_type, input.composition_type),
                                 String{input.name},
                                 input.attributes.location.value());
#endif
            }
            auto& outputs = entryPoint.output_variables;
            builder.NumberOfOutputs(outputs.size());
            for (auto& output : outputs)
            {
                builder.AddOutput(GetShaderDataType(output.component_type, output.composition_type),
                                  String{output.name},
                                  output.attributes.location.value());
            }

            const auto& resourceBindings = inspector.GetResourceBindings(entryPoint.name);
            BeeCoreTrace("Number of Resource Bindings: {}", resourceBindings.size());
            if (inspector.has_error())
            {
                BeeCoreError("Inspector has error: {}", inspector.error());
            }
            for (const auto& uniform : resourceBindings)
            {
                builder.AddUniform(
                    GetShaderUniformDataType(uniform.resource_type), uniform.bind_group, uniform.binding, uniform.size);
            }
            break; // support for only main entry point
        }
        /*auto resourceBindings = inspector.GetResourceBindings("main");
        for (auto& resource : resourceBindings)
        {

        }*/
        return builder.Build();
    }
#endif
    BufferLayout ShaderConverter::GenerateLayout(in<std::vector<uint32_t>> spirv, BufferLayoutBuilder& builder)
    {
        BeeCoreTrace("Generating buffer layout for spirv shader");
        return builder.Build();
#if 0
        tint::spirv::reader::Options options;
        options.allowed_features = tint::wgsl::AllowedFeatures::Everything();
        auto shader = tint::spirv::reader::Read(spirv, options);
        tint::inspector::Inspector inspector(shader);
        auto entryPoints = inspector.GetEntryPoints();
        for (auto& entryPoint : entryPoints)
        {
            if (entryPoint.name != "main") // support for only main entry point
                continue;
            auto& inputs = entryPoint.input_variables;
            builder.NumberOfInputs(inputs.size());
            for (auto& input : inputs)
            {
                builder.AddInput(GetShaderDataType(input.component_type, input.composition_type),
                                 String{input.name},
                                 input.attributes.location.value());
            }
            auto& outputs = entryPoint.output_variables;
            builder.NumberOfOutputs(outputs.size());
            for (auto& output : outputs)
            {
                builder.AddOutput(GetShaderDataType(output.component_type, output.composition_type),
                                  String{output.name},
                                  output.attributes.location.value());
            }

            const auto& resourceBindings = inspector.GetResourceBindings(entryPoint.name);
            BeeCoreTrace("Number of Resource Bindings: {}", resourceBindings.size());
            if (inspector.has_error())
            {
                BeeCoreError("Inspector has error: {}", inspector.error());
            }
            for (const auto& uniform : resourceBindings)
            {
                builder.AddUniform(
                    GetShaderUniformDataType(uniform.resource_type), uniform.bind_group, uniform.binding, uniform.size);
            }
            break; // support for only main entry point
        }
        /*auto resourceBindings = inspector.GetResourceBindings("main");
        for (auto& resource : resourceBindings)
        {

        }*/
        return builder.Build();
#endif
    }
    BufferLayout ShaderConverter::GenerateLayout(in<String> wgsl, in<std::string> path, BufferLayoutBuilder& builder)
    {
#if defined(BEE_COMPILE_WEBGPU)
        BeeCoreTrace("Generating buffer layout for wgsl shader");
        tint::Source::File file(path, wgsl);
        auto shader = tint::reader::wgsl::Parse(&file);
        tint::inspector::Inspector inspector(&shader);
        auto entryPoints = inspector.GetEntryPoints();
        for (auto& entryPoint : entryPoints)
        {
            if (entryPoint.name != "main") // support for only main entry point
                continue;
            auto& inputs = entryPoint.input_variables;
            builder.NumberOfInputs(inputs.size());
            for (auto& input : inputs)
            {
                builder.AddInput(GetShaderDataType(input.component_type, input.composition_type),
                                 input.name,
                                 input.location_attribute);
            }
            const auto& uniforms = inspector.GetUniformBufferResourceBindings(entryPoint.name);
            BeeCoreTrace("Number of Uniforms: {}", uniforms.size());
            if (inspector.has_error())
            {
                BeeCoreError("Inspector has error: {}", inspector.error());
            }
            const auto& resourceBindings = inspector.GetResourceBindings(entryPoint.name);
            BeeCoreTrace("Number of Resource Bindings: {}", resourceBindings.size());
            if (inspector.has_error())
            {
                BeeCoreError("Inspector has error: {}", inspector.error());
            }
            for (const auto& uniform : uniforms)
            {
                BeeCoreTrace("Uniform: {}, {}, {}, {}, {}, {}",
                             uniform.bind_group,
                             uniform.binding,
                             ToString(uniform.image_format),
                             ToString(uniform.resource_type),
                             uniform.size,
                             uniform.size_no_padding);
            }
            break; // support for only main entry point
        }
        /*auto resourceBindings = inspector.GetResourceBindings("main");
        for (auto& resource : resourceBindings)
        {

        }*/
#else
        BeeCoreError("Trying to generate buffer layout for WGSL without WebGPU");
#endif
        return builder.Build();
    }
    int extractIntegerWords(const String& str)
    {
        std::stringstream ss;

        /* Storing the whole string into string stream */
        ss << str;

        /* Running loop till the end of the stream */
        std::string temp;
        int found;
        while (!ss.eof())
        {
            /* extracting word by word from stream */
            ss >> temp;

            /* Checking the given word is integer or not */
            if (std::stringstream(temp) >> found)
                return found;

            /* To save from space at the end of string */
            temp = "";
        }
        return -1;
    }

    bool ShaderConverter::AnalyzeGLSL(const ShaderType& stage, out<BufferLayoutBuilder> layout, out<String> glsl)
    {
        BeeCoreTrace("Analyzing GLSL shader");
        size_t pos = 0;
        size_t endpos = glsl.size() - 1;
        do
        {
            pos = glsl.find("instanced", pos + 1);
            if (pos == String::npos)
            {
                break;
            }
            size_t location = glsl.find('=', pos);
            if (location == String::npos)
            {
                BeeCoreError("Failed to find '=' after 'instanced'");
                return false;
            }
            size_t closeBracket = glsl.find(')', location);
            if (closeBracket == String::npos)
            {
                BeeCoreError("Failed to find ')' after '='");
                return false;
            }
            String locationStr = glsl.substr(location, closeBracket - location);
            int locationInt = extractIntegerWords(locationStr);
            if (locationInt == -1)
            {
                BeeCoreError("Failed to extract integer from '{}'", locationStr);
                return false;
            }
            BeeCoreTrace("Found instanced attribute at location {}", locationInt);
            layout.RegisterInstancedLocation(locationInt);
        } while (pos != endpos);

        size_t index = 0;
        while (true)
        {
            /* Locate the substring to replace. */
            index = glsl.find("instanced", index);
            if (index == String::npos)
                break;

            /* Make the replacement. */
            glsl.replace(index, 9, "");

            /* Advance index forward so the next iteration doesn't pick it up as well. */
            index += 1;
        }

        BeeCoreTrace("GLSL analyzed successfully");
        return true;
    }
} // namespace BeeEngine
