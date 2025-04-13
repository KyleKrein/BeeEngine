//
// Created by alexl on 06.06.2023.
//

#pragma once

#include <glm/glm.hpp>
#include <Scene/Entity.h>
#include <cmath>
#include <numbers>

namespace BeeEngine::Math
{
    consteval float32_t PI()
    {
        return std::numbers::pi_v<float32_t>;
    }
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

            float GetSignedDistanceToPlane(const glm::vec3& point) const { return glm::dot(Normal, point) - Distance; }
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
            virtual bool IsOnFrustum(const Frustum& camFrustum, const glm::mat4& modelTransform) const = 0;
        };

        struct Sphere final : public Volume
        {
            glm::vec3 center{0.f, 0.f, 0.f};
            float radius{0.f};

            Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}

            Sphere() = default;

            bool IsOnFrustum(const Frustum& camFrustum, const glm::mat4& transform) const final;
            ;

            bool IsOnOrForwardPlane(const Plane& plane) const
            {
                return plane.GetSignedDistanceToPlane(center) > -radius;
            }
        };

        Frustum CreateFrustumFromCamera(const glm::vec3& position,
                                        const glm::vec3& front,
                                        const glm::vec3& right,
                                        const glm::vec3& up,
                                        float aspect,
                                        float fovY,
                                        float zNear,
                                        float zFar);

    } // namespace Cameras
    class Radians; // Предварительное объявление, чтобы класс Degrees "знал" о существовании класса Radians

    class Degrees
    {
    public:
        explicit Degrees(float32_t deg = 0.0f) : degrees(deg) {}
        Degrees(const Radians& rad);
        Degrees& operator=(const Radians& rad);
        Degrees& operator=(float32_t deg);

        float32_t ToRadians() const { return degrees * (PI() / 180.0f); }

        String ToString() const { return ::BeeEngine::ToString(degrees) + "°"; }

        friend std::ostream& operator<<(std::ostream& os, const Degrees& deg)
        {
            os << deg.ToString();
            return os;
        }

    private:
        float32_t degrees;
    };

    class Radians
    {
    public:
        explicit Radians(float32_t rad = 0.0f) : radians(rad) {}
        Radians(const Degrees& deg);
        Radians& operator=(const Degrees& deg);
        Radians& operator=(float32_t rad);

        float32_t ToDegrees() const { return radians * (180.0 / PI()); }

        String ToString() const { return ::BeeEngine::ToString(radians) + "rad"; }

        friend std::ostream& operator<<(std::ostream& os, const Radians& rad)
        {
            os << rad.ToString();
            return os;
        }

    private:
        float32_t radians;
    };
    DecomposedTransform DecomposeTransform(const glm::mat4& transform);
    // Пример функции для перевода локальных координат в глобальные
    glm::mat4 ToGlobalTransform(Entity child);
    glm::mat4 ToLocalTransform(Entity child);
    glm::mat4 ToLocalTransform(Entity child, glm::mat4 transform);
    bool RayIntersectsTriangle(const glm::vec3& rayOrigin,
                               const glm::vec3& rayVector,
                               const glm::vec3& vertex0,
                               const glm::vec3& vertex1,
                               const glm::vec3& vertex2);

    glm::vec3 GetScaleFromMatrix(const glm::mat4& mat);

    glm::mat4 GetTransformFromTo(const glm::vec3& start, const glm::vec3& end, float lineWidth);
} // namespace BeeEngine::Math
