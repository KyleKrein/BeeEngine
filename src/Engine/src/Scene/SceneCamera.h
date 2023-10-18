//
// Created by alexl on 03.06.2023.
//

#pragma once

#include "Core/Cameras/Camera.h"
#include "Core/Reflection.h"
#include "Serialization/ISerializer.h"

namespace BeeEngine
{

    class SceneCamera: public Camera
    {
        REFLECT()
    public:
        enum class CameraType
        {
            Perspective = 0,
            Orthographic = 1
        };

        SceneCamera();
        void SetOrthographic(float size, float nearClip, float farClip);
        void SetPerspective(float verticalFOV, float nearClip, float farClip);

        void SetViewportSize(uint32_t width, uint32_t height);

        CameraType GetProjectionType() noexcept { return m_Type; }
        void SetProjectionType(CameraType type) noexcept;

        float GetOrthographicSize() const noexcept { return m_OrthographicSize; }
        void SetOrthographicSize(float size) noexcept;
        float GetOrthographicNearClip() const noexcept { return m_OrthographicNear; }
        void SetOrthographicNearClip(float nearClip) noexcept;
        float GetOrthographicFarClip() const noexcept { return m_OrthographicFar; }
        void SetOrthographicFarClip(float farClip) noexcept;

        float GetPerspectiveVerticalFOV() const noexcept { return m_VerticalFOV; }
        void SetPerspectiveVerticalFOV(float verticalFOV) noexcept;
        float GetPerspectiveNearClip() const noexcept { return m_PerspectiveNear; }
        void SetPerspectiveNearClip(float nearClip) noexcept;
        float GetPerspectiveFarClip() const noexcept { return m_PerspectiveFar; }
        void SetPerspectiveFarClip(float farClip) noexcept;


        void SetAspectRatio(float d);

        float GetAspectRatio();
        template<typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer & Serialization::Key("Projection Type") & Serialization::Value(m_Type);
            serializer & Serialization::Key("Orthographic Size") & Serialization::Value(m_OrthographicSize);
            serializer & Serialization::Key("Orthographic Near") & Serialization::Value(m_OrthographicNear);
            serializer & Serialization::Key("Orthographic Far") & Serialization::Value(m_OrthographicFar);
            serializer & Serialization::Key("Vertical FOV") & Serialization::Value(m_VerticalFOV);
            serializer & Serialization::Key("Perspective Near") & Serialization::Value(m_PerspectiveNear);
            serializer & Serialization::Key("Perspective Far") & Serialization::Value(m_PerspectiveFar);
        }

    private:
        void RecalculateProjectionMatrix();
        CameraType m_Type = CameraType::Orthographic;
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f;
        float m_OrthographicFar = 1.0f;

        float m_AspectRatio = 0.0f;

        float m_VerticalFOV = glm::radians(45.0f);
        float m_PerspectiveNear = 0.01f;
        float m_PerspectiveFar = 1000.0f;
    };
}
