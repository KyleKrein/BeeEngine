//
// Created by Александр Лебедев on 15.10.2023.
//

#pragma once
#include "Core/String.h"
#include "Core/UUID.h"
#include "Font.h"
#include "Renderer/BindingSet.h"
#include "Renderer/Model.h"
#include "TextRenderingConfiguration.h"
#include "gsl/gsl"
#include <glm.hpp>
#include <vector>

namespace BeeEngine
{
    class SceneTreeRenderer
    {
        friend class SceneRenderer;

    public:
        SceneTreeRenderer(glm::mat4 cameraTransform, BindingSet* textBindingSet);

        void AddEntity(glm::mat4 transform,
                       bool isTransparent,
                       Model& model,
                       const std::vector<BindingSet*>& bindingSets,
                       gsl::span<byte> instancedData);
        void AddText(const UTF8String& text,
                     Font* font,
                     const glm::mat4& transform,
                     const TextRenderingConfiguration& configuration,
                     int32_t entityID);
        auto&& GetAllEntities() { return std::move(m_AllEntities); }

        struct Entity
        {
            glm::mat4 Transform;
            Model* Model;
            std::vector<BindingSet*> BindingSets;
            std::vector<byte> InstancedData;
        };

    private:
        std::vector<Entity> m_AllEntities;
        BindingSet* m_TextBindingSet;
        glm::mat4 m_CameraTransform;
    };
} // namespace BeeEngine
