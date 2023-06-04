//
// Created by alexl on 03.06.2023.
//

#pragma once
#include "glm.hpp"
namespace BeeEngine
{
    class Camera
    {
    public:
        Camera() = default;
        explicit Camera(const glm::mat4 &projection) noexcept
        : m_ProjectionMatrix(projection)
        {}

        virtual ~Camera() = default;
        [[nodiscard]] inline const glm::mat4& GetProjectionMatrix() const {return m_ProjectionMatrix;}

    protected:
        glm::mat4 m_ProjectionMatrix {1.0f};
    };
}
