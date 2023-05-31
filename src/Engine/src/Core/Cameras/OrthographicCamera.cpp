//
// Created by alexl on 14.05.2023.
//

#include "OrthographicCamera.h"
#include "ext/matrix_clip_space.hpp"
#include "ext/matrix_transform.hpp"
#include "Debug/Instrumentor.h"


namespace BeeEngine
{

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
    m_ViewMatrix(glm::mat4(1.0f)),
    m_ViewProjectionMatrix(m_ProjectionMatrix * m_ViewMatrix)
    {}

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        BEE_PROFILE_FUNCTION();
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        RecalculateViewMatrix();
    }

    void OrthographicCamera::RecalculateViewMatrix()
    {
        BEE_PROFILE_FUNCTION();
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
