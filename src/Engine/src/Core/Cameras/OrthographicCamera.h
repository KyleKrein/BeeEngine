//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Camera.h"

namespace BeeEngine
{
    class OrthographicCamera: public Camera
    {
public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

        [[nodiscard]] const glm::mat4& GetViewProjectionMatrix() const override { return m_ViewProjectionMatrix; }

        [[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        [[nodiscard]] float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

private:
        glm::mat4 m_ViewProjectionMatrix;
        float m_Rotation = 0;
        glm::vec3 m_Position = { 0, 0, 0 };
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;

        void RecalculateViewMatrix();
    };
}
