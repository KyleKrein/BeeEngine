//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Core/AssetManagement/MeshSource.h"
#include "Core/Cameras/Camera.h"
#include "Core/Color4.h"
#include "Core/Math/Math.h"
#include "Core/Reflection.h"
#include "Core/TypeSequence.h"
#include "Core/UUID.h"
#include "NativeScriptFactory.h"
#include "Renderer/Font.h"
#include "Renderer/Material.h"
#include "Renderer/MaterialData.h"
#include "Renderer/Mesh.h"
#include "Renderer/TextRenderingConfiguration.h"
#include "Renderer/Texture.h"
#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Scripting/GameScript.h"
#include "Scripting/MObject.h"
#include "Serialization/ISerializer.h"
#include "ext/matrix_transform.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

namespace BeeEngine
{
    struct UUIDComponent
    {
        UUID ID;

        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"UUID"} & Serialization::Value{ID};
        }

    private:
        REFLECT()
    };
    struct TagComponent
    {
        String Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        explicit TagComponent(const std::string& tag) : Tag(tag) {}
        explicit TagComponent(std::string&& tag) : Tag(std::move(tag)) {}

        operator String&() { return Tag; }
        operator const String&() const { return Tag; }
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer & Tag;
        }

    private:
        REFLECT()
    };

    struct TransformComponent
    {
        glm::vec3 Translation = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        explicit TransformComponent(const glm::vec3& translate) : Translation(translate) {}

        [[nodiscard]] glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        void SetTransform(const glm::mat4& transform) noexcept
        {
            auto [translation, rotation, scale] = Math::DecomposeTransform(transform);
            Translation = translation;
            Rotation += rotation - Rotation;
            Scale = scale;
        }
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            using Serialization::Marker;
            serializer& Serialization::Key{"Translation"} & Serialization::Value{Translation};
            serializer& Serialization::Key{"Rotation"} & Serialization::Value{Rotation};
            serializer& Serialization::Key{"Scale"} & Serialization::Value{Scale};
        }

    private:
        REFLECT()
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        // explicit CameraComponent(const glm::mat4& projection): Camera(projection) {}

        operator class Camera &() { return Camera; }
        operator const class Camera &() const { return Camera; }
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer & Camera;
            serializer& Serialization::Key{"Primary"} & Serialization::Value{Primary};
            serializer& Serialization::Key{"Fixed Aspect Ratio"} & Serialization::Value{FixedAspectRatio};
        }

    private:
        REFLECT()
    };

    struct SpriteRendererComponent
    {
        Color4 Color = Color4::White;
        AssetHandle TextureHandle;
        float TilingFactor = 1.0f;
        bool HasTexture = false;

        [[nodiscard]] Texture2D* Texture(const Locale::Localization& locale) const
        {
            BeeExpects(HasTexture);
            BeeExpects(AssetManager::IsAssetHandleValid(TextureHandle));
            return &AssetManager::GetAsset<Texture2D>(TextureHandle, locale);
        }

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"Color"} & Serialization::Value{Color};
            serializer& Serialization::Key{"Texture Handle"} & Serialization::Value{TextureHandle};
            serializer& Serialization::Key{"Tiling Factor"} & Serialization::Value{TilingFactor};
            serializer& Serialization::Key{"Has Texture"} & Serialization::Value{HasTexture};
        }
    };

    struct CircleRendererComponent
    {
        Color4 Color = Color4::White;
        float Thickness = 1.0f;
        float Fade = 0.005f;
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"Color"} & Serialization::Value{Color};
            serializer& Serialization::Key{"Thickness"} & Serialization::Value{Thickness};
            serializer& Serialization::Key{"Fade"} & Serialization::Value{Fade};
        }
    };

    struct TextRendererComponent
    {
        TextRenderingConfiguration Configuration;
        AssetHandle FontHandle = EngineAssetRegistry::OpenSansRegular;
        std::string Text;

        Font& Font(const String& locale) const { return AssetManager::GetAsset<BeeEngine::Font>(FontHandle, locale); }
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"Configuration"} & Serialization::Value{Configuration};
            serializer& Serialization::Key{"Font Handle"} & Serialization::Value{FontHandle};
            serializer& Serialization::Key{"Text"} & Serialization::Value{Text};
        }
    };

    /*struct MeshComponent
    {
        Ref<Mesh> Mesh = nullptr;
        Ref<Material> Material = nullptr;

        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(const Ref<BeeEngine::Mesh>& mesh): Mesh(mesh) {}
        MeshComponent(Ref<BeeEngine::Mesh>&& mesh): Mesh(std::move(mesh)) {}
        MeshComponent(const Ref<BeeEngine::Material>& material): Material(material) {}
        MeshComponent(Ref<BeeEngine::Material>&& material): Material(std::move(material)) {}

        operator Ref<BeeEngine::Mesh>&() { return Mesh; }
        operator const Ref<BeeEngine::Mesh>&() const { return Mesh; }
    };*/

    struct ScriptComponent
    {
        class MClass* Class = nullptr;
        std::vector<GameScriptField> EditableFields;
        void SetClass(class MClass* mClass) noexcept;
        /*template<typename Archive>
        void Serialize(Archive& serializer)
        {
            //serializer & Serialization::Key{"Class"} & Serialization::Value{Class};
            //serializer & Serialization::Key{"Editable Fields"} & Serialization::Value{EditableFields};
        }*/
    };

    struct NativeScriptComponent
    {
        std::string Name = "";
        ScriptableEntity* Instance = nullptr;

        // ScriptableEntity*(*InstantiateScript)();
        // void (*DestroyScript)();
        std::function<ScriptableEntity*(const char*)> InstantiateScript = [](const char* name)
        { return NativeScriptFactory::GetInstance().Create(name); };
        ;
        std::function<void(NativeScriptComponent&)> DestroyScript = [](NativeScriptComponent& script)
        {
            delete script.Instance;
            script.Instance = nullptr;
        };

        /*
        std::function<void()> OnCreateFunction;
        std::function<void()> OnDestroyFunction;
        std::function<void()> OnUpdateFunction;
         */

        template <typename T>
            requires std::derived_from<T, ScriptableEntity>
        void Bind()
        {
            InstantiateScript = [](const char* name) { return new T(); };
            DestroyScript = [](NativeScriptComponent& script)
            {
                delete script.Instance;
                script.Instance = nullptr;
            };
            /*
            OnCreateFunction = [&]() { ((T*)Instance)->OnCreate();};
            OnDestroyFunction = [&]() { ((T*)Instance)->OnDestroy();};
            OnUpdateFunction = [&]() { ((T*)Instance)->OnUpdate();};
            */
        }

        ~NativeScriptComponent()
        {
            if (Instance)
            {
                DestroyScript(*this);
            }
        }
    };

    // Physics
    struct RigidBody2DComponent
    {
        enum class BodyType
        {
            Static = 0,
            Dynamic = 1,
            Kinematic = 2
        };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        void* RuntimeBody = nullptr;
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"Type"} & Serialization::Value{Type};
            serializer& Serialization::Key{"Fixed Rotation"} & Serialization::Value{FixedRotation};
        }
        /*float Mass = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.5f;
        bool IsKinematic = false;
        bool UseGravity = true;
        bool FreezeRotation = false;
        bool FreezePosition = false;
        bool FreezeRotationX = false;
        bool FreezeRotationY = false;
        bool FreezeRotationZ = false;
        bool FreezePositionX = false;
        bool FreezePositionY = false;
        bool FreezePositionZ = false;*/
    };
    struct BoxCollider2DComponent
    {
        enum class ColliderType
        {
            Box = 0,
            Circle = 1
        };
        ColliderType Type = ColliderType::Box;
        glm::vec2 Offset = glm::vec2(0.0f);
        glm::vec2 Size = glm::vec2(0.5f);

        // move to physics material
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        void* RuntimeFixture = nullptr;
        template <typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer& Serialization::Key{"Type"} & Serialization::Value{Type};
            serializer& Serialization::Key{"Offset"} & Serialization::Value{Offset};
            serializer& Serialization::Key{"Size"} & Serialization::Value{Size};
            serializer& Serialization::Key{"Density"} & Serialization::Value{Density};
            serializer& Serialization::Key{"Friction"} & Serialization::Value{Friction};
            serializer& Serialization::Key{"Restitution"} & Serialization::Value{Restitution};
            serializer& Serialization::Key{"Restitution Threshold"} & Serialization::Value{RestitutionThreshold};
        }
    };

    struct HierarchyComponent
    {
        Entity Parent = Entity::Null;
        std::vector<Entity> Children;
        /*void Serialize(Serialization::ISerializer& serializer)
        {
            serializer & Children;
        }*/
    };

    struct MeshComponent
    {
        // Ref<Mesh> Mesh = nullptr;
        // Ref<Material> Material = nullptr;
        AssetHandle MeshSourceHandle;
        bool HasMeshes = false;
        MaterialInstance MaterialInstance;

        [[nodiscard]] MeshSource* MeshSource() const
        {
            BeeExpects(HasMeshes);
            BeeExpects(AssetManager::IsAssetHandleValid(MeshSourceHandle));
            return &AssetManager::GetAsset<BeeEngine::MeshSource>(MeshSourceHandle);
        }
    };

    using AllComponents = TypeSequence<TransformComponent,
                                       TagComponent,
                                       UUIDComponent,
                                       CameraComponent,
                                       SpriteRendererComponent,
                                       CircleRendererComponent,
                                       TextRendererComponent,
                                       ScriptComponent,
                                       NativeScriptComponent,
                                       RigidBody2DComponent,
                                       BoxCollider2DComponent,
                                       HierarchyComponent,
                                       MeshComponent>;
} // namespace BeeEngine
