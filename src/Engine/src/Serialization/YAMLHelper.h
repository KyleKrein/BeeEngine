#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/Color4.h"
#include "Core/UUID.h"
#include <yaml-cpp/yaml.h>
namespace YAML
{
    template <>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };
    template <>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template <>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template <>
    struct convert<BeeEngine::Color4>
    {
        static Node encode(const BeeEngine::Color4& rhs)
        {
            Node node;
            node.push_back(rhs.R());
            node.push_back(rhs.G());
            node.push_back(rhs.B());
            node.push_back(rhs.A());
            return node;
        }

        static bool decode(const Node& node, BeeEngine::Color4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            rhs = BeeEngine::Color4::FromNormalized(
                node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
            return true;
        }
    };

    template <>
    struct convert<BeeEngine::AssetHandle>
    {
        static Node encode(const BeeEngine::AssetHandle& rhs)
        {
            Node node;
            node.push_back((uint64_t)rhs.RegistryID);
            node.push_back((uint64_t)rhs.AssetID);
            return node;
        }

        static bool decode(const Node& node, BeeEngine::AssetHandle& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;
            rhs = BeeEngine::AssetHandle(node[0].as<uint64_t>(), node[1].as<uint64_t>());
            return true;
        }
    };

    template <>
    struct convert<BeeEngine::UUID>
    {
        static Node encode(const BeeEngine::UUID& uuid)
        {
            Node node;
            node.push_back((uint64_t)uuid);
            return node;
        }

        static bool decode(const Node& node, BeeEngine::UUID& uuid)
        {
            uuid = node.as<uint64_t>();
            return true;
        }
    };
} // namespace YAML

namespace BeeEngine
{
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const AssetHandle& handle)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << (uint64_t)handle.RegistryID << (uint64_t)handle.AssetID << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const Color4& color)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << color.R() << color.G() << color.B() << color.A() << YAML::EndSeq;
        return out;
    }
    inline YAML::Emitter& operator<<(YAML::Emitter& out, const UUID& uuid)
    {
        out << YAML::Flow;
        out << (uint64_t)uuid;
        return out;
    }
} // namespace BeeEngine