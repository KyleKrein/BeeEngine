//
// Created by alexl on 06.06.2023.
//


#include "Math.h"
#include "ext/scalar_constants.hpp"
#include "gtc/epsilon.hpp"
#include "gtx/matrix_decompose.inl"

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
}