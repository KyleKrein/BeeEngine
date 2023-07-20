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

namespace BeeEngine
{
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
        Texture2D* Texture = nullptr;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        explicit SpriteRendererComponent(const Color4& color): Color(color) {}
        explicit SpriteRendererComponent(Texture2D& texture): Texture(&texture) {}

        operator Texture2D&() { return *Texture; }
        operator Texture2D&() const { return *Texture; }
    };

    struct MeshComponent
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
    };

    struct NativeScriptComponent
    {
        ScriptableEntity* Instance = nullptr;

        ScriptableEntity*(*InstantiateScript)();
        void (*DestroyScript)();

        /*
        std::function<void()> OnCreateFunction;
        std::function<void()> OnDestroyFunction;
        std::function<void()> OnUpdateFunction;
         */

        template<typename T>
        requires std::derived_from<T, ScriptableEntity>
        void Bind()
        {
            InstantiateScript = []() { return new T();};
            DestroyScript = [this]() {delete Instance; Instance = nullptr; };
            /*
            OnCreateFunction = [&]() { ((T*)Instance)->OnCreate();};
            OnDestroyFunction = [&]() { ((T*)Instance)->OnDestroy();};
            OnUpdateFunction = [&]() { ((T*)Instance)->OnUpdate();};
            */
        }
    };
}