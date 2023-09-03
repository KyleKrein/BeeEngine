//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "glm.hpp"
#include "Renderer/Texture.h"
#include "Core/Cameras/Camera.h"
#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "ext/matrix_transform.hpp"
#include "gtx/quaternion.hpp"
#include "Core/Math/Math.h"
#include "Core/Color4.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "NativeScriptFactory.h"
#include "Core/UUID.h"
#include "Scripting/MObject.h"
#include "Scripting/GameScript.h"
#include "Renderer/TextRenderingConfiguration.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Renderer/Font.h"
#include "Core/TypeSequence.h"
#include "Core/AssetManagement/AssetManager.h"

namespace BeeEngine
{
    struct UUIDComponent
    {
        UUID ID;
    };
    struct TagComponent
    {
        String Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        explicit TagComponent(const std::string& tag): Tag(tag) {}
        explicit TagComponent(std::string&& tag): Tag(std::move(tag)) {}

        operator String&() { return Tag; }
        operator const String&() const { return Tag; }
    };

    struct TransformComponent
    {
        glm::vec3 Translation = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        explicit TransformComponent(const glm::vec3& translate): Translation(translate) {}

        [[nodiscard]] glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        void SetTransform(glm::mat4 &transform) noexcept
        {
            auto [translation, rotation, scale] = Math::DecomposeTransform(transform);
            Translation = translation;
            Rotation += rotation - Rotation;
            Scale = scale;
        }
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        //explicit CameraComponent(const glm::mat4& projection): Camera(projection) {}

        operator class Camera&() { return Camera; }
        operator const class Camera&() const { return Camera; }
    };

    struct SpriteRendererComponent
    {
        Color4 Color = Color4::White;
        AssetHandle TextureHandle;
        float TilingFactor = 1.0f;
        bool HasTexture = false;

        Texture2D* Texture() const
        {
            BeeExpects(HasTexture);
            BeeExpects(AssetManager::IsAssetHandleValid(TextureHandle));
            return &AssetManager::GetAsset<Texture2D>(TextureHandle);
        }

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

        operator Texture2D&() { return AssetManager::GetAsset<Texture2D>(TextureHandle); }
        operator Texture2D&() const { return AssetManager::GetAsset<Texture2D>(TextureHandle); }
    };

    struct CircleRendererComponent
    {
        Color4 Color = Color4::White;
        float Thickness = 1.0f;
        float Fade = 0.005f;
    };

    struct TextRendererComponent
    {
        TextRenderingConfiguration Configuration;
        AssetHandle FontHandle = EngineAssetRegistry::OpenSansRegular;
        std::string Text;

        Font& Font() const
        {
            return AssetManager::GetAsset<BeeEngine::Font>(FontHandle);
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
    };

    struct NativeScriptComponent
    {
        std::string Name = "";
        ScriptableEntity* Instance = nullptr;

        //ScriptableEntity*(*InstantiateScript)();
        //void (*DestroyScript)();
        std::function<ScriptableEntity*(const char*)> InstantiateScript = [](const char* name) { return NativeScriptFactory::GetInstance().Create(name); };;
        std::function<void(NativeScriptComponent&)> DestroyScript = [](NativeScriptComponent& script) { delete script.Instance; script.Instance = nullptr; };

        /*
        std::function<void()> OnCreateFunction;
        std::function<void()> OnDestroyFunction;
        std::function<void()> OnUpdateFunction;
         */

        template<typename T>
        requires std::derived_from<T, ScriptableEntity>
        void Bind()
        {
            InstantiateScript = [](const char* name) { return new T();};
            DestroyScript = [](NativeScriptComponent& script) { delete script.Instance; script.Instance = nullptr; };
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

    //Physics
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

        //move to physics material
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        void* RuntimeFixture = nullptr;
    };

    using AllComponents =
            TypeSequence<TransformComponent, TagComponent, UUIDComponent, CameraComponent,
            SpriteRendererComponent, CircleRendererComponent, TextRendererComponent, /*MeshComponent,*/ ScriptComponent, NativeScriptComponent,
            RigidBody2DComponent, BoxCollider2DComponent>;
}