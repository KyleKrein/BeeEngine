//
// Created by alexl on 06.06.2023.
//

#pragma once

#include "glm.hpp"
#include <Scene/Entity.h>

namespace BeeEngine::Math
{
    struct DecomposedTransform
    {
        glm::vec3 Translation;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };
    namespace Cameras
    {
        struct Plane
        {
            // unit vector
            glm::vec3 Normal = {0.f, 1.f, 0.f};

            // distance from origin to the nearest point in the plane
            float Distance = 0.f;

            float GetSignedDistanceToPlane(const glm::vec3 &point) const
            {
                return glm::dot(Normal, point) - Distance;
            }
        };

        struct Frustum
        {
            Plane TopFace;
            Plane BottomFace;

            Plane RightFace;
            Plane LeftFace;

            Plane FarFace;
            Plane NearFace;
        };

        struct Volume
        {
            virtual bool IsOnFrustum(const Frustum &camFrustum,
                                     const glm::mat4 &modelTransform) const = 0;
        };

        struct Sphere final : public Volume
        {
            glm::vec3 center{0.f, 0.f, 0.f};
            float radius{0.f};

            Sphere(const glm::vec3 &center, float radius)
                    : center(center), radius(radius)
            {}

            Sphere() = default;

            bool IsOnFrustum(const Frustum &camFrustum, const glm::mat4 &transform) const final;;

            bool IsOnOrForwardPlane(const Plane &plane) const
            {
                return plane.GetSignedDistanceToPlane(center) > -radius;
            }
        };

        Frustum CreateFrustumFromCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &right,
                                const glm::vec3 &up, float aspect, float fovY,
                                float zNear, float zFar);

    }
    DecomposedTransform DecomposeTransform(const glm::mat4& transform);
    // Пример функции для перевода локальных координат в глобальные
    glm::mat4 ToGlobalTransform(Entity child);
    glm::mat4 ToLocalTransform(Entity child);
    glm::mat4 ToLocalTransform(Entity child, glm::mat4 transform);
    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayVector,
                               const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2);

    glm::vec3 GetScaleFromMatrix(const glm::mat4 &mat);

    glm::mat4 GetTransformFromTo(const glm::vec3 &start, const glm::vec3 &end, float lineWidth);
}