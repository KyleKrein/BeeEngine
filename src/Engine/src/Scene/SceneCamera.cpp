//
// Created by alexl on 03.06.2023.
//

#include <gsl/gsl>
#include "SceneCamera.h"
#include "ext/matrix_clip_space.hpp"


namespace BeeEngine
{

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_Type = CameraType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;

        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_Type = CameraType::Perspective;
        m_VerticalFOV = verticalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;

        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        const float newAspectRatio = gsl::narrow_cast<float>(width) / gsl::narrow_cast<float>(height);
        if (m_AspectRatio != newAspectRatio)
        {
            m_AspectRatio = newAspectRatio;
            RecalculateProjectionMatrix();
        }
    }

    void SceneCamera::RecalculateProjectionMatrix()
    {
        switch (m_Type)
        {
            case CameraType::Perspective:
                m_ProjectionMatrix = glm::perspective(m_VerticalFOV, m_AspectRatio,
                                                        m_PerspectiveNear, m_PerspectiveFar);
                break;
            case CameraType::Orthographic:
                const float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
                const float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
                const float orthoBottom = -m_OrthographicSize * 0.5f;
                const float orthoTop = m_OrthographicSize * 0.5f;

                m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight,
                                                orthoBottom, orthoTop,
                                                m_OrthographicNear, m_OrthographicFar);
                break;
        }
    }

    SceneCamera::SceneCamera()
    {
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetProjectionType(CameraType type) noexcept
    {
        m_Type = type;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetOrthographicSize(float size) noexcept
    {
        m_OrthographicSize = size;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetOrthographicNearClip(float nearClip) noexcept
    {
        m_OrthographicNear = nearClip;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetOrthographicFarClip(float farClip) noexcept
    {
        m_OrthographicFar = farClip;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetPerspectiveVerticalFOV(float verticalFOV) noexcept
    {
        m_VerticalFOV = verticalFOV;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetPerspectiveNearClip(float nearClip) noexcept
    {
        m_PerspectiveNear = nearClip;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetPerspectiveFarClip(float farClip) noexcept
    {
        m_PerspectiveFar = farClip;
        RecalculateProjectionMatrix();
    }

    void SceneCamera::SetAspectRatio(float d)
    {
        m_AspectRatio = d;
        RecalculateProjectionMatrix();
    }

    float SceneCamera::GetAspectRatio()
    {
        return m_AspectRatio;
    }

REFLECT_STRUCT_BEGIN(SceneCamera)
    REFLECT_STRUCT_MEMBER(m_Type)
    REFLECT_STRUCT_MEMBER(m_OrthographicSize)
    REFLECT_STRUCT_MEMBER(m_OrthographicNear)
    REFLECT_STRUCT_MEMBER(m_OrthographicFar)
    REFLECT_STRUCT_MEMBER(m_AspectRatio)
    REFLECT_STRUCT_MEMBER(m_VerticalFOV)
    REFLECT_STRUCT_MEMBER(m_PerspectiveNear)
    REFLECT_STRUCT_MEMBER(m_PerspectiveFar)
    /*REFLECT_MEMBERS_END()
    REFLECT_METHODS_BEGIN()
    REFLECT_METHOD(SetOrthographic)
    REFLECT_METHOD(SetPerspective)
    REFLECT_METHOD(SetViewportSize)
    REFLECT_METHOD(SetProjectionType)
    REFLECT_METHOD(SetOrthographicSize)
    REFLECT_METHOD(SetOrthographicNearClip)
    REFLECT_METHOD(SetOrthographicFarClip)
    REFLECT_METHOD(SetPerspectiveVerticalFOV)
    REFLECT_METHOD(SetPerspectiveNearClip)
    REFLECT_METHOD(SetPerspectiveFarClip)
    REFLECT_METHOD(SetAspectRatio)
    REFLECT_METHOD(GetAspectRatio)
    REFLECT_METHOD(GetProjectionType)
    REFLECT_METHOD(GetOrthographicSize)
    REFLECT_METHOD(GetOrthographicNearClip)
    REFLECT_METHOD(GetOrthographicFarClip)
    REFLECT_METHOD(GetPerspectiveVerticalFOV)
    REFLECT_METHOD(GetPerspectiveNearClip)
    REFLECT_METHOD(GetPerspectiveFarClip)
    REFLECT_METHOD(GetProjectionMatrix)*/
REFLECT_STRUCT_END()

REFLECT_ENUM(SceneCamera::CameraType)
}
