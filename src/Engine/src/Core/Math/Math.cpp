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
        return ToLocalTransform(child, localMatrix);
    }
    glm::mat4 ToLocalTransform(Entity child, glm::mat4 localMatrix)
    {
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

    glm::vec3 GetScaleFromMatrix(const glm::mat4 &mat)
    {
        glm::vec3 globalScale;
        globalScale.x = glm::length(mat[0]); // Длина первой колонны
        globalScale.y = glm::length(mat[1]); // Длина второй колонны
        globalScale.z = glm::length(mat[2]); // Длина третьей колонны
        return globalScale;
    }

    glm::mat4 GetTransformFromTo(const glm::vec3 &start, const glm::vec3 &end, float lineWidth)
    {
        // Вычислить вектор направления и его длину
        glm::vec3 direction = glm::normalize(end - start);
        float length = glm::length(end - start);

        // Вычислить вектор направления для начальной позиции линии (например, вдоль оси Z)
        glm::vec3 initialDirection(0.0f, 1.0f, 0.0f);

        // Вычислить кватернион вращения между начальным направлением и желаемым направлением
        glm::quat rotation = glm::rotation(initialDirection, direction);
        glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

        // Вычислить среднюю точку между start и end для правильного позиционирования
        glm::vec3 midpoint = (start + end) / 2.0f;

        // Создать матрицу трансформации
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), midpoint);  // Переместить к середине линии
        transform *= rotationMatrix;  // Применить вращение
        transform = glm::scale(transform, glm::vec3(lineWidth, length, lineWidth));  // Применить масштабирование

        return transform;
    }
    Cameras::Frustum
    Cameras::CreateFrustumFromCamera(const glm::vec3 &position, const glm::vec3 &front, const glm::vec3 &right,
                                           const glm::vec3 &up, float aspect, float fovY, float zNear, float zFar)
    {
        Cameras::Frustum frustum;
        float tangent = tan(glm::radians(fovY) / 2.0f);
        float nearHeight = zNear * tangent;
        float nearWidth = nearHeight * aspect;
        float farHeight = zFar * tangent;
        float farWidth = farHeight * aspect;

        // Вычисление вершин фрустума в локальных координатах камеры
        glm::vec3 ntl = glm::vec3(-nearWidth, nearHeight, -zNear); //TODO: must be -zNear and -zFar everywhere
        glm::vec3 ntr = glm::vec3(nearWidth, nearHeight, -zNear);
        glm::vec3 nbl = glm::vec3(-nearWidth, -nearHeight, -zNear);
        glm::vec3 nbr = glm::vec3(nearWidth, -nearHeight, -zNear);

        glm::vec3 ftl = glm::vec3(-farWidth, farHeight, -zFar);
        glm::vec3 ftr = glm::vec3(farWidth, farHeight, -zFar);
        glm::vec3 fbl = glm::vec3(-farWidth, -farHeight, -zFar);
        glm::vec3 fbr = glm::vec3(farWidth, -farHeight, -zFar);

        frustum.NearFace.Normal = glm::normalize(front * zNear);
        frustum.FarFace.Normal = glm::normalize(front * zFar);
        frustum.LeftFace.Normal = glm::normalize(glm::cross(up, nbl - ntl));
        frustum.RightFace.Normal = glm::normalize(glm::cross(up, ntr - nbr));
        frustum.TopFace.Normal = glm::normalize(glm::cross(right, ntr - ntl));
        frustum.BottomFace.Normal = glm::normalize(glm::cross(right, nbr - nbl));

        frustum.NearFace.Distance = glm::dot(frustum.NearFace.Normal, ntl);
        frustum.FarFace.Distance = glm::dot(frustum.FarFace.Normal, ftl);
        frustum.LeftFace.Distance = glm::dot(frustum.LeftFace.Normal, ntl);
        frustum.RightFace.Distance = glm::dot(frustum.RightFace.Normal, ntr);
        frustum.TopFace.Distance = glm::dot(frustum.TopFace.Normal, ntl);
        frustum.BottomFace.Distance = glm::dot(frustum.BottomFace.Normal, nbl);

        return frustum;
    }

    bool Cameras::Sphere::IsOnFrustum(const Cameras::Frustum &camFrustum, const glm::mat4 &transform) const
    {
        //Get global scale is computed by doing the magnitude of
        //X, Y and Z model matrix's column.
        const glm::vec3 globalScale = Math::GetScaleFromMatrix(transform);

        //Get our global center with process it with the global model matrix of our transform
        const glm::vec3 globalCenter{ transform * glm::vec4(center, 1.f) };

        //To wrap correctly our shape, we need the maximum scale scalar.
        const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        //Max scale is assuming for the diameter. So, we need the half to apply it to our radius
        Cameras::Sphere globalSphere{globalCenter, radius * (maxScale * 0.5f)};

        //Check Firstly the result that have the most chance
        //to faillure to avoid to call all functions.
        return (globalSphere.IsOnOrForwardPlane(camFrustum.LeftFace) &&
                globalSphere.IsOnOrForwardPlane(camFrustum.RightFace) &&
                globalSphere.IsOnOrForwardPlane(camFrustum.FarFace) &&
                globalSphere.IsOnOrForwardPlane(camFrustum.NearFace) &&
                globalSphere.IsOnOrForwardPlane(camFrustum.TopFace) &&
                globalSphere.IsOnOrForwardPlane(camFrustum.BottomFace));
    }
}