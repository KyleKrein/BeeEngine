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
#include <vector>
#include "Scripting/GameScript.h"
#include "Scripting/MUtils.h"

namespace YAML
{
    template<>
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

    template<>
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
}

namespace BeeEngine
{

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case MType::FieldType:          \
				out << field.GetData<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
    case MType::FieldType:                   \
	{                                                  \
		Type fieldData = scriptField["Data"].as<Type>();    \
		field->SetData(fieldData);                  \
		break;                                         \
	}

    static std::string RigidBodyTypeToString(RigidBody2DComponent::BodyType type)
    {
        switch (type)
        {
            case RigidBody2DComponent::BodyType::Static: return "Static";
            case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
            case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
        }
        return "Unknown";
    }
    static RigidBody2DComponent::BodyType RigidBodyTypeFromString(const std::string& type)
    {
        if (type == "Static") return RigidBody2DComponent::BodyType::Static;
        if (type == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
        if (type == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;
        return RigidBody2DComponent::BodyType::Static;
    }


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
                        script.SetClass(&ScriptingEngine::GetGameScript(className));

                        const auto& scriptFields = scriptComponent["Fields"];
                        auto& fields = script.EditableFields;
                        std::unordered_map<std::string_view, GameScriptField*> fieldsMap;
                        for (auto& field : fields)
                        {
                            fieldsMap[field.GetMField().GetName()] = &field;
                        }
                        for( auto& scriptField : scriptFields)
                        {
                            std::string name = scriptField["Name"].as<std::string>();
                            if(!fieldsMap.contains(name))
                            {
                                BeeCoreWarn("Script field {0} not found in script {1}", name, className);
                                continue;
                            }
                            std::string typeString = scriptField["Type"].as<std::string>();
                            auto type = MUtils::StringToMType(typeString);
                            auto* field = fieldsMap.at(name);

                            switch (type)
                            {
                                READ_SCRIPT_FIELD(Single, float);
                                READ_SCRIPT_FIELD(Double, double);
                                READ_SCRIPT_FIELD(Boolean, bool);
                                READ_SCRIPT_FIELD(Char, char);
                                READ_SCRIPT_FIELD(Byte, uint8_t);
                                READ_SCRIPT_FIELD(Int16, int16_t);
                                READ_SCRIPT_FIELD(Int32, int32_t);
                                READ_SCRIPT_FIELD(Int64, int64_t);
                                READ_SCRIPT_FIELD(SByte, int8_t);
                                READ_SCRIPT_FIELD(UInt16, uint16_t);
                                READ_SCRIPT_FIELD(UInt32, uint32_t);
                                READ_SCRIPT_FIELD(UInt64, uint64_t);
                                READ_SCRIPT_FIELD(Vector2, glm::vec2);
                                READ_SCRIPT_FIELD(Vector3, glm::vec3);
                                READ_SCRIPT_FIELD(Vector4, glm::vec4);
                                READ_SCRIPT_FIELD(Color, Color4);
                                READ_SCRIPT_FIELD(Entity, UUID);

                            }
                        }
                    }
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if(spriteRendererComponent)
                {
                    auto& spriteRenderer = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    spriteRenderer.Color = spriteRendererComponent["Color"].as<Color4>();
                }
                auto rigidBody2DComponent = entity["RigidBody2DComponent"];
                if(rigidBody2DComponent)
                {
                    auto& rb2dcomp = deserializedEntity.AddComponent<RigidBody2DComponent>();
                    rb2dcomp.Type = RigidBodyTypeFromString(rigidBody2DComponent["Type"].as<std::string>());
                    rb2dcomp.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
                }
                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
                if(boxCollider2DComponent)
                {
                    auto& boxCollider = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                    boxCollider.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
                    boxCollider.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
                    boxCollider.Density = boxCollider2DComponent["Density"].as<float>();
                    boxCollider.Friction = boxCollider2DComponent["Friction"].as<float>();
                    boxCollider.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                    boxCollider.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
                }
            }
        }
    }

    void SceneSerializer::DeserializeBinary(const std::filesystem::path& filepath)
    {
        BeeCoreAssert(false, "Not implemented yet");
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
        return out;
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
            auto& fields = scriptComponent.EditableFields;
            if(fields.size() > 0)
            {
                out << YAML::Key << "Fields" << YAML::Value;
                out << YAML::BeginSeq;
                for(auto& field : fields)
                {
                    auto& mField = field.GetMField();
                    MType type = mField.GetType();
                    if(mField.IsStatic() ||
                    type == MType::None || type == MType::Object
                    || type == MType::String || type == MType::Array
                    || type == MType::List || type == MType::Dictionary
                    || type == MType::Ptr || type == MType::Void)
                        continue;
                    out << YAML::BeginMap;
                    out << YAML::Key << "Name" << YAML::Value << mField.GetName();
                    out << YAML::Key << "Type" << YAML::Value << MUtils::MTypeToString(type);
                    out << YAML::Key << "Data" << YAML::Value;

                    switch (type)
                    {
                        WRITE_SCRIPT_FIELD(Single, float);
                        WRITE_SCRIPT_FIELD(Double, double);
                        WRITE_SCRIPT_FIELD(Boolean, bool);
                        WRITE_SCRIPT_FIELD(Char, char);
                        WRITE_SCRIPT_FIELD(Byte, uint8_t);
                        WRITE_SCRIPT_FIELD(Int16, int16_t);
                        WRITE_SCRIPT_FIELD(Int32, int32_t);
                        WRITE_SCRIPT_FIELD(Int64, int64_t);
                        WRITE_SCRIPT_FIELD(SByte, int8_t);
                        WRITE_SCRIPT_FIELD(UInt16, uint16_t);
                        WRITE_SCRIPT_FIELD(UInt32, uint32_t);
                        WRITE_SCRIPT_FIELD(UInt64, uint64_t);
                        WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
                        WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
                        WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
                        WRITE_SCRIPT_FIELD(Color, Color4);
                        WRITE_SCRIPT_FIELD(Entity, UUID);
                    }
                    out << YAML::EndMap;
                }
                out << YAML::EndSeq;
            }

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

        if(entity.HasComponent<RigidBody2DComponent>())
        {
            out << YAML::Key << "RigidBody2DComponent";
            out << YAML::BeginMap;
            auto& rigidBody2dComponent = entity.GetComponent<RigidBody2DComponent>();
            out << YAML::Key << "Type" << YAML::Value << RigidBodyTypeToString(rigidBody2dComponent.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rigidBody2dComponent.FixedRotation;
            out << YAML::EndMap;
        }

        if(entity.HasComponent<BoxCollider2DComponent>())
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;
            auto& boxCollider2dComponent = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Offset" << YAML::Value << boxCollider2dComponent.Offset;
            out << YAML::Key << "Size" << YAML::Value << boxCollider2dComponent.Size;
            out << YAML::Key << "Density" << YAML::Value << boxCollider2dComponent.Density;
            out << YAML::Key << "Friction" << YAML::Value << boxCollider2dComponent.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << boxCollider2dComponent.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider2dComponent.RestitutionThreshold;
            out << YAML::EndMap;
        }

        out << YAML::EndMap; //Entity
    }
}
