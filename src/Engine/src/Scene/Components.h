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
    struct Texture2DComponent
    {
        Ref<Texture2D> Texture;

        Texture2DComponent() = default;
        Texture2DComponent(const Texture2DComponent&) = default;
        explicit Texture2DComponent(const Ref<Texture2D>& texture): Texture(texture) {}
        explicit Texture2DComponent(Ref<Texture2D>&& texture): Texture(std::move(texture)) {}

        operator Ref<Texture2D>&() { return Texture; }
        operator const Ref<Texture2D>&() const { return Texture; }
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