//
// Created by alexl on 06.06.2023.
//


#include "Math.h"
#include "ext/scalar_constants.hpp"
#include "gtc/epsilon.hpp"
#include "gtx/matrix_decompose.inl"
#include "Scene/Components.h"

namespace BeeEngine::Math
{

    DecomposedTransform DecomposeTransform(const glm::mat4 &transform)
    {

        using namespace glm;
        using T = float;

        mat4 localTransform = transform;

        //Normalize the matrix
        if (epsilonEqual(localTransform[3][3], static_cast<T>(0), epsilon<T>()))
        {
            return {};
        }

        //First, isolate perspective.  This is the messiest.
        if(
            epsilonNotEqual(localTransform[0][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(localTransform[1][3], static_cast<T>(0), epsilon<T>()) ||
            epsilonNotEqual(localTransform[2][3], static_cast<T>(0), epsilon<T>())
                )
        {
            //Clear the perspective partition
            localTransform[0][3] = static_cast<T>(0);
            localTransform[1][3] = static_cast<T>(0);
            localTransform[2][3] = static_cast<T>(0);
            localTransform[3][3] = static_cast<T>(1);
        }

        //Next take care of translation (easy).
        vec3 translation(localTransform[3]);
        localTransform[3] = vec4(0, 0, 0, localTransform[3].w);

        vec3 row[3];

        //Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
        {
            for (length_t j = 0; j < 3; ++j)
            {
                row[i][j] = localTransform[i][j];
            }
        }

        // Compute X scale factor and normalize first row.

        vec3 scale;
        scale.x = length(row[0]);
        row[0] = detail::scale(row[0], static_cast<T>(1));
        scale.y = length(row[1]);
        row[1] = detail::scale(row[1], static_cast<T>(1));
        scale.z = length(row[2]);
        row[2] = detail::scale(row[2], static_cast<T>(1));

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
#if 0
        pdum3 = cross(row[1], row[2]);
        if (dot(row[0], pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                row[i] *= static_cast<T>(-1);
            }
        }
#endif

        // Now, get the rotations out, as described in the gem.
        glm::vec3 rotation;
        rotation.y = asin(-row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(row[1][2], row[2][2]);
            rotation.z = atan2(row[0][1], row[0][0]);
        }
        else {
            rotation.x = atan2(-row[2][0], row[1][1]);
            rotation.z = 0;
        }

        return {translation, rotation, scale};
    }

    glm::mat4 ToLocalTransform(Entity child)
    {
        glm::mat4 localMatrix = child.GetComponent<TransformComponent>().GetTransform();
        auto currentEntity = child;
        while(currentEntity.HasParent())
        {
            currentEntity = currentEntity.GetParent();
            auto inverseMatrix = glm::inverse(currentEntity.GetComponent<TransformComponent>().GetTransform());
            localMatrix = inverseMatrix * localMatrix;
        }
        return localMatrix;
    }

    glm::mat4 ToGlobalTransform(Entity child)
    {
        glm::mat4 globalMatrix(1.0f);
        auto currentEntity = child;
        globalMatrix = globalMatrix * child.GetComponent<TransformComponent>().GetTransform();
        while(currentEntity.HasParent())
        {
            currentEntity = currentEntity.GetParent();
            auto matrix = currentEntity.GetComponent<TransformComponent>().GetTransform();
            globalMatrix = matrix * globalMatrix;
        }
        return globalMatrix;
    }

    bool RayIntersectsTriangle(const glm::vec3 &rayOrigin, const glm::vec3 &rayVector, const glm::vec3 &vertex0,
                               const glm::vec3 &vertex1, const glm::vec3 &vertex2)
    {
        // Compute vectors
        glm::vec3 edge1 = vertex1 - vertex0;
        glm::vec3 edge2 = vertex2 - vertex0;

        // Compute determinant
        glm::vec3 h = glm::cross(rayVector, edge2);
        float a = glm::dot(edge1, h);

        // Parallel
        if (a > -0.00001f && a < 0.00001f)
            return false;

        float f = 1.0f / a;
        glm::vec3 s = rayOrigin - vertex0;
        float u = f * glm::dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return false;

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(rayVector, q);

        if (v < 0.0f || u + v > 1.0f)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * glm::dot(edge2, q);

        if (t > 0.00001f) // ray intersection
            return true;

        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }
}