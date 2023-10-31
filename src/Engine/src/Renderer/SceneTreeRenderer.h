//
// Created by Александр Лебедев on 15.10.2023.
//

#pragma once
#include <glm.hpp>
#include "Core/UUID.h"
#include "Renderer/Model.h"
#include "Renderer/BindingSet.h"
#include <vector>
#include "gsl/gsl"
#include "TextRenderingConfiguration.h"
#include "Core/String.h"
#include "Font.h"
namespace BeeEngine
{
    class SceneTreeRenderer
    {
        friend class SceneRenderer;
    public:
        SceneTreeRenderer(glm::mat4 cameraTransform, BindingSet* textBindingSet);

        void AddEntity(glm::mat4 transform, bool isTransparent, Model& model, const std::vector<BindingSet*>& bindingSets, gsl::span<byte> instancedData);
        void AddText(const UTF8String& text, Font* font, const glm::mat4& transform, const TextRenderingConfiguration& configuration, int32_t entityID);
        void Render();
    private:
        struct Entity
        {
            glm::mat4 Transform;
            Model* Model;
            std::vector<BindingSet*> BindingSets;
            std::vector<byte> InstancedData;
        };
        std::vector<Entity> m_NotTransparent;
        std::vector<Entity> m_Transparent;
        BindingSet* m_TextBindingSet;
        glm::mat4 m_CameraTransform;
    };
}
