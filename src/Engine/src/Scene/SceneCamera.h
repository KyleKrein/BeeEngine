//
// Created by alexl on 03.06.2023.
//

#pragma once

#include "Core/Cameras/Camera.h"

namespace BeeEngine
{

    class SceneCamera: public Camera
    {
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
