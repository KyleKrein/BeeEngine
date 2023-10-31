//
// Created by alexl on 25.10.2023.
//

#pragma once
#include "FrameBuffer.h"
#include "Scene/Scene.h"
#include "glm/glm.hpp"
namespace BeeEngine
{
    class SceneRenderer
    {
    private:
        static Model* s_RectModel;
        static Model* s_CircleModel;
        static Texture2D* s_BlankTexture;
    public:
        static void Init();
        static void RenderScene(Scene& scene, FrameBuffer& frameBuffer, const String& locale, const glm::mat4& viewProjectionMatrix);
        static void RenderScene(Scene& scene, FrameBuffer& frameBuffer, const String& locale);
    };

} // BeeEngine
