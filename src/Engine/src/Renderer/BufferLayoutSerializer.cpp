#include "BufferLayoutSerializer.hpp"
#include "Serialization/YAMLHelper.h"

namespace BeeEngine
{
    static const char* ToString(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::NoneData:
                return "NoneData";
            case ShaderDataType::Half:
                return "Half";
            case ShaderDataType::Float:
                return "Float";
            case ShaderDataType::Float2:
                return "Float2";
            case ShaderDataType::Float3:
                return "Float3";
            case ShaderDataType::Float4:
                return "Float4";
            case ShaderDataType::Mat3:
                return "Mat3";
            case ShaderDataType::Mat4:
                return "Mat4";
            case ShaderDataType::UInt:
                return "UInt";
            case ShaderDataType::UInt2:
                return "UInt2";
            case ShaderDataType::UInt3:
                return "UInt3";
            case ShaderDataType::UInt4:
                return "UInt4";
            case ShaderDataType::Int:
                return "Int";
            case ShaderDataType::Int2:
                return "Int2";
            case ShaderDataType::Int3:
                return "Int3";
            case ShaderDataType::Int4:
                return "Int4";
            case ShaderDataType::Bool:
                return "Bool";
            default:
                return "Unknown";
        }
    }

    static ShaderDataType ShaderDataTypeFromString(const String& str)
    {
        if (str == "NoneData")
        {
            return ShaderDataType::NoneData;
        }
        if (str == "Half")
        {
            return ShaderDataType::Half;
        }
        if (str == "Float")
        {
            return ShaderDataType::Float;
        }
        if (str == "Float2")
        {
            return ShaderDataType::Float2;
        }
        if (str == "Float3")
        {
            return ShaderDataType::Float3;
        }
        if (str == "Float4")
        {
            return ShaderDataType::Float4;
        }
        if (str == "Mat3")
        {
            return ShaderDataType::Mat3;
        }
        if (str == "Mat4")
        {
            return ShaderDataType::Mat4;
        }
        if (str == "UInt")
        {
            return ShaderDataType::UInt;
        }
        if (str == "UInt2")
        {
            return ShaderDataType::UInt2;
        }
        if (str == "UInt3")
        {
            return ShaderDataType::UInt3;
        }
        if (str == "UInt4")
        {
            return ShaderDataType::UInt4;
        }
        if (str == "Int")
        {
            return ShaderDataType::Int;
        }
        if (str == "Int2")
        {
            return ShaderDataType::Int2;
        }
        if (str == "Int3")
        {
            return ShaderDataType::Int3;
        }
        if (str == "Int4")
        {
            return ShaderDataType::Int4;
        }
        if (str == "Bool")
        {
            return ShaderDataType::Bool;
        }
        return ShaderDataType::NoneData;
    }

    static const char* ToString(ShaderUniformDataType type)
    {
        switch (type)
        {
            case ShaderUniformDataType::Unknown:
                return "Unknown";
            case ShaderUniformDataType::Data:
                return "Data";
            case ShaderUniformDataType::Sampler:
                return "Sampler";
            case ShaderUniformDataType::SampledTexture:
                return "SampledTexture";
            default:
                return "Unknown";
        }
    }

    static ShaderUniformDataType ShaderUniformDataTypeFromString(const String& str)
    {
        if (str == "Unknown")
        {
            return ShaderUniformDataType::Unknown;
        }
        if (str == "Data")
        {
            return ShaderUniformDataType::Data;
        }
        if (str == "Sampler")
        {
            return ShaderUniformDataType::Sampler;
        }
        if (str == "SampledTexture")
        {
            return ShaderUniformDataType::SampledTexture;
        }
        return ShaderUniformDataType::Unknown;
    }

    String BufferLayoutSerializer::Serialize(const BufferLayout& layout)
    {
        YAML::Emitter out;

        out << YAML::BeginMap;

        // Serialize Input Elements
        out << YAML::Key << "InputElements" << YAML::Value << YAML::BeginSeq;
        for (const auto& element : layout.GetInputElements())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << element.GetName().c_str();
            out << YAML::Key << "Type" << YAML::Value << ToString(element.GetType());
            out << YAML::Key << "Location" << YAML::Value << element.GetLocation();
            out << YAML::Key << "Size" << YAML::Value << element.GetSize();
            out << YAML::Key << "Offset" << YAML::Value << element.GetOffset();
            out << YAML::Key << "Normalized" << YAML::Value << element.IsNormalized();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        // Serialize Instanced Elements
        out << YAML::Key << "InstancedElements" << YAML::Value << YAML::BeginSeq;
        for (const auto& element : layout.GetInstancedElements())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << element.GetName().c_str();
            out << YAML::Key << "Type" << YAML::Value << ToString(element.GetType());
            out << YAML::Key << "Location" << YAML::Value << element.GetLocation();
            out << YAML::Key << "Size" << YAML::Value << element.GetSize();
            out << YAML::Key << "Offset" << YAML::Value << element.GetOffset();
            out << YAML::Key << "Normalized" << YAML::Value << element.IsNormalized();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        // Serialize Uniform Elements
        out << YAML::Key << "UniformElements" << YAML::Value << YAML::BeginSeq;
        for (const auto& element : layout.GetUniformElements())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Type" << YAML::Value << ToString(element.GetType());
            out << YAML::Key << "BindingSet" << YAML::Value << element.GetBindingSet();
            out << YAML::Key << "Location" << YAML::Value << element.GetLocation();
            out << YAML::Key << "Size" << YAML::Value << element.GetSize();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        // Serialize Output Elements
        out << YAML::Key << "OutputElements" << YAML::Value << YAML::BeginSeq;
        for (const auto& element : layout.GetOutputElements())
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << element.GetName().c_str();
            out << YAML::Key << "Type" << YAML::Value << ToString(element.GetType());
            out << YAML::Key << "Location" << YAML::Value << element.GetLocation();
            out << YAML::Key << "Size" << YAML::Value << element.GetSize();
            out << YAML::Key << "Offset" << YAML::Value << element.GetOffset();
            out << YAML::Key << "Normalized" << YAML::Value << element.IsNormalized();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;

        return out.c_str();
    }

    BufferLayout BufferLayoutSerializer::Deserialize(const String& str)
    {
        YAML::Node node = YAML::Load(str.c_str());

        std::vector<BufferElement> inElements;
        std::vector<BufferElement> instancedElements;
        std::vector<BufferUniformElement> uniformElements;
        std::vector<BufferElement> outElements;

        // Deserialize Input Elements
        auto inElementsNode = node["InputElements"];
        if (inElementsNode)
        {
            for (auto elementNode : inElementsNode)
            {
                auto name = String{elementNode["Name"].as<std::string>()};
                ShaderDataType type = ShaderDataTypeFromString(String{elementNode["Type"].as<std::string>()});
                uint32_t location = elementNode["Location"].as<uint32_t>();
                uint32_t size = elementNode["Size"].as<uint32_t>();
                uint32_t offset = elementNode["Offset"].as<uint32_t>();
                bool normalized = elementNode["Normalized"].as<bool>();

                BufferElement element(type, name, location, normalized);
                element.SetOffset(offset);
                inElements.push_back(element);
            }
        }

        // Deserialize Instanced Elements
        auto instancedElementsNode = node["InstancedElements"];
        if (instancedElementsNode)
        {
            for (auto elementNode : instancedElementsNode)
            {
                String name = String{elementNode["Name"].as<std::string>()};
                ShaderDataType type = ShaderDataTypeFromString(String{elementNode["Type"].as<std::string>()});
                uint32_t location = elementNode["Location"].as<uint32_t>();
                uint32_t size = elementNode["Size"].as<uint32_t>();
                uint32_t offset = elementNode["Offset"].as<uint32_t>();
                bool normalized = elementNode["Normalized"].as<bool>();

                BufferElement element(type, name, location, normalized);
                element.SetOffset(offset);
                instancedElements.push_back(element);
            }
        }

        // Deserialize Uniform Elements
        auto uniformElementsNode = node["UniformElements"];
        if (uniformElementsNode)
        {
            for (auto uniformElementNode : uniformElementsNode)
            {
                ShaderUniformDataType type =
                    ShaderUniformDataTypeFromString(String{uniformElementNode["Type"].as<std::string>()});
                uint32_t bindingSet = uniformElementNode["BindingSet"].as<uint32_t>();
                uint32_t location = uniformElementNode["Location"].as<uint32_t>();
                uint32_t size = uniformElementNode["Size"].as<uint32_t>();

                BufferUniformElement uniformElement(type, bindingSet, location, size);
                uniformElements.push_back(uniformElement);
            }
        }

        // Deserialize Output Elements
        auto outElementsNode = node["OutputElements"];
        if (outElementsNode)
        {
            for (auto outputElementNode : outElementsNode)
            {
                String name = String{outputElementNode["Name"].as<std::string>()};
                ShaderDataType type = ShaderDataTypeFromString(String{outputElementNode["Type"].as<std::string>()});
                uint32_t location = outputElementNode["Location"].as<uint32_t>();
                uint32_t size = outputElementNode["Size"].as<uint32_t>();
                uint32_t offset = outputElementNode["Offset"].as<uint32_t>();
                bool normalized = outputElementNode["Normalized"].as<bool>();

                BufferElement outputElement(type, name, location, normalized);
                outputElement.SetOffset(offset);
                outElements.push_back(outputElement);
            }
        }

        return {
            std::move(inElements), std::move(instancedElements), std::move(uniformElements), std::move(outElements)};
    }
} // namespace BeeEngine