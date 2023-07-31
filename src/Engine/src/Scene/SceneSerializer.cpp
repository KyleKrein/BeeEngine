//
// Created by alexl on 04.06.2023.
//

#include <fstream>
#include "SceneSerializer.h"
#include "Core/Logging/Log.h"
#include "yaml-cpp/yaml.h"
#include "Entity.h"
#include "Components.h"
#include "Core/Color4.h"
#include "Scripting/MClass.h"
#include "Scripting/ScriptingEngine.h"

namespace YAML
{
    template<>
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

    template<>
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

    template<>
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
            rhs = BeeEngine::Color4::FromNormalized(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
            return true;
        }
    };
}

namespace BeeEngine
{

    SceneSerializer::SceneSerializer(Ref<Scene> &scene)
            : m_Scene(scene)
    {}

    void SceneSerializer::Serialize(const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.each([&](auto entityID)
        {
            Entity entity = {EntityID{entityID}, m_Scene.get()};
            if (!entity)
                return;
            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeBinary(const std::filesystem::path& filepath)
    {
        BeeCoreAssert(false, "Not implemented yet");
    }

    void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath.string());
        if (!data["Scene"])
            return;
        std::string sceneName = data["Scene"].as<std::string>();
        BeeCoreTrace("Deserializing scene '{0}'", sceneName);
        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();
                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                {
                    name = tagComponent["Tag"].as<std::string>();
                }

                BeeCoreTrace("Deserialized entity with ID = {0}, name = {1}", uuid, name);

                Entity deserializedEntity = m_Scene->CreateEntityWithUUID({uuid},name);
                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& transform = deserializedEntity.GetComponent<TransformComponent>();
                    transform.Translation = transformComponent["Translation"].as<glm::vec3>();
                    transform.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    transform.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto cameraComponent = entity["CameraComponent"];
                if(cameraComponent)
                {
                    auto& camera = deserializedEntity.AddComponent<CameraComponent>();
                    auto& cameraProps = camera.Camera;
                    auto cameraInComponent = cameraComponent["Camera"];
                    cameraProps.SetProjectionType((SceneCamera::CameraType)(cameraInComponent["ProjectionType"].as<int>()));
                    cameraProps.SetPerspectiveVerticalFOV(cameraInComponent["PerspectiveVerticalFOV"].as<float>());
                    cameraProps.SetPerspectiveNearClip(cameraInComponent["PerspectiveNearClip"].as<float>());
                    cameraProps.SetPerspectiveFarClip(cameraInComponent["PerspectiveFarClip"].as<float>());

                    cameraProps.SetOrthographicSize(cameraInComponent["OrthographicSize"].as<float>());
                    cameraProps.SetOrthographicNearClip(cameraInComponent["OrthographicNearClip"].as<float>());
                    cameraProps.SetOrthographicFarClip(cameraInComponent["OrthographicFarClip"].as<float>());
                    cameraProps.SetAspectRatio(cameraInComponent["AspectRatio"].as<float>());

                    camera.Primary = cameraComponent["Primary"].as<bool>();
                    camera.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }

                auto scriptComponent = entity["ScriptComponent"];
                if(scriptComponent)
                {
                    auto& script = deserializedEntity.AddComponent<ScriptComponent>();
                    auto className = scriptComponent["FullName"].as<std::string>();
                    if(ScriptingEngine::HasGameScript(className))
                    {
                        script.Class = &ScriptingEngine::GetGameScript(className);
                    }
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if(spriteRendererComponent)
                {
                    auto& spriteRenderer = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    spriteRenderer.Color = spriteRendererComponent["Color"].as<Color4>();
                }
            }
        }
    }

    void SceneSerializer::DeserializeBinary(const std::filesystem::path& filepath)
    {
        BeeCoreAssert(false, "Not implemented yet");
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const Color4& color)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << color.R() << color.G() << color.B() << color.A() << YAML::EndSeq;
        return out;
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter &out, Entity entity)
    {
        BeeCoreAssert(entity.HasComponent<UUIDComponent>(), "Entity has no UUID component");
        out << YAML::BeginMap; //Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;
            out << YAML::EndMap;
        }

        if(entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            auto& transform = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << transform.Scale;
            out << YAML::EndMap;
        }

        if(entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            auto& camera = entity.GetComponent<CameraComponent>();
            out << YAML::Key << "Camera" << YAML::Value;

            out << YAML::BeginMap;//Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.Camera.GetProjectionType();
            out << YAML::Key << "PerspectiveVerticalFOV" << YAML::Value << camera.Camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.Camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.Camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.Camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.Camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.Camera.GetOrthographicFarClip();
            out << YAML::Key << "AspectRatio" << YAML::Value << camera.Camera.GetAspectRatio();
            out << YAML::EndMap;//Camera

            out << YAML::Key << "Primary" << YAML::Value << camera.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << camera.FixedAspectRatio;
            out << YAML::EndMap;
        }

        if(entity.HasComponent<ScriptComponent>())
        {
            out << YAML::Key << "ScriptComponent";
            out << YAML::BeginMap;
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            out << YAML::Key << "FullName" << YAML::Value << (scriptComponent.Class ? scriptComponent.Class->GetFullName() : "");
            out << YAML::EndMap;
        }

        if(entity.HasComponent<SpriteRendererComponent>())
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap;
            auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << spriteRenderer.Color;
            out << YAML::EndMap;
        }

        out << YAML::EndMap; //Entity
    }
}
