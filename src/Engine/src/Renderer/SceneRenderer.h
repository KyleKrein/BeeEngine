//
// Created by alexl on 25.10.2023.
//

#pragma once
#include "FrameBuffer.h"
#include "Scene/Scene.h"
#include "glm/glm.hpp"
#include "Core/Math/Math.h"
namespace BeeEngine
{
    template<typename T>
    concept CameraClass = requires(T a)
    {
        { a.GetAspectRatio() } -> std::convertible_to<float>;
        { a.GetVerticalFOV() } -> std::convertible_to<float>;
        { a.GetNearClip() } -> std::convertible_to<float>;
        { a.GetFarClip() } -> std::convertible_to<float>;
    };
    class SceneRenderer
    {
    private:
        static Model* s_RectModel;
        static Model* s_CircleModel;
        static Texture2D* s_BlankTexture;
    public:
        static void Init();
        static void RenderScene(Scene& scene, FrameBuffer& frameBuffer, const String& locale, const glm::mat4& viewProjectionMatrix, const Math::Cameras::Frustum& frustum);
        static void RenderScene(Scene& scene, FrameBuffer& frameBuffer, const String& locale);

        static void RenderPhysicsColliders(Scene& scene, FrameBuffer& frameBuffer, const glm::mat4& viewProjectionMatrix);
        static void RenderPhysicsColliders(Scene& scene, FrameBuffer& frameBuffer, BindingSet& cameraBindingSet);
        template<CameraClass T>
        static void RenderScene(Scene& scene, FrameBuffer& frameBuffer, const String& locale, const T& camera, const glm::mat4& viewProjectionMatrix, const glm::vec3& position, const glm::vec3& forwardDirection, const glm::vec3& upDirection, const glm::vec3& rightDirection)
        {
            Math::Cameras::Frustum frustum = Math::Cameras::CreateFrustumFromCamera(position, forwardDirection,
                                                      rightDirection, upDirection,
                                                      camera.GetAspectRatio(), camera.GetVerticalFOV(),
                                                      camera.GetNearClip(), camera.GetFarClip());
            RenderScene(scene, frameBuffer, locale, viewProjectionMatrix, frustum);
        }
    };

} // BeeEngine