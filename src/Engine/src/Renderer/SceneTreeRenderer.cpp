//
// Created by Александр Лебедев on 15.10.2023.
//

#include "SceneTreeRenderer.h"
#include "Core/Application.h"
#include "FrameBuffer.h"
#include "MSDFData.h"
#include "Renderer.h"
#include "RenderingQueue.h"
#include <ranges>

namespace BeeEngine
{
    SceneTreeRenderer::SceneTreeRenderer(glm::mat4 cameraTransform, BindingSet* textBindingSet)
        : m_CameraTransform(cameraTransform), m_TextBindingSet(textBindingSet)
    {
    }

    void SceneTreeRenderer::AddEntity(glm::mat4 transform,
                                      bool isTransparent,
                                      Model& model,
                                      const std::vector<BindingSet*>& bindingSets,
                                      std::span<byte> instancedData)
    {
        auto& vec = isTransparent ? m_Transparent : m_Opaque;
        std::vector<byte> instancedDataVector(instancedData.size());
        memcpy(instancedDataVector.data(), instancedData.data(), instancedData.size());
        vec.emplace_back(Entity{transform, &model, bindingSets, std::move(instancedDataVector)});
    }
    struct TextInstancedData
    {
        glm::vec2 TexCoord0;
        glm::vec2 TexCoord1;
        glm::vec2 TexCoord2;
        glm::vec2 TexCoord3;
        glm::vec3 PositionOffset0;
        glm::vec3 PositionOffset1;
        glm::vec3 PositionOffset2;
        glm::vec3 PositionOffset3;
        Color4 ForegroundColor;
        Color4 BackgroundColor;
        int32_t EntityID;
    };
    void SceneTreeRenderer::AddText(const UTF8String& text,
                                    Font* font,
                                    const glm::mat4& transform,
                                    const TextRenderingConfiguration& config,
                                    int32_t entityID)
    {
        BeeExpects(IsValidString(text));
        auto& textModel = Application::GetInstance().GetAssetManager().GetModel("Renderer_Font");

        auto& fontGeometry = font->GetMSDFData().FontGeometry;
        auto& metrics = fontGeometry.getMetrics();
        auto& atlasTexture = font->GetAtlasTexture();
        auto& atlasBindingSet = font->GetAtlasBindingSet();

        double x = 0.0;
        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0; //-fsScale * metrics.ascenderY;

        const auto spaceGlyph = fontGeometry.getGlyph(' ');

        UTF8StringView textView(text);
        auto it = textView.begin();
        auto end = textView.end();

        while (it != text.end())
        {
            char32_t character = *it++;

            if (character == '\r')
                continue;

            if (character == '\n')
            {
                x = 0;
                y -= fsScale * metrics.lineHeight + config.LineSpacing;
                continue;
            }
            if (character == ' ')
            {
                if (it != end)
                {
                    double advance = spaceGlyph->getAdvance();
                    char32_t nextCharacter = *it;
                    fontGeometry.getAdvance(advance, character, nextCharacter);

                    x += fsScale * advance + config.KerningOffset;
                }

                continue;
            }
            if (character == '\t')
            {
                if (it != end)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        double advance = spaceGlyph->getAdvance();
                        char32_t nextCharacter = ' ';
                        fontGeometry.getAdvance(advance, character, nextCharacter);

                        x += fsScale * advance + config.KerningOffset;
                    }
                    double advance = spaceGlyph->getAdvance();
                    char32_t nextCharacter = *it;
                    fontGeometry.getAdvance(advance, character, nextCharacter);

                    x += fsScale * advance + config.KerningOffset;
                }
                continue;
            }
            auto glyph = fontGeometry.getGlyph(character);
            if (!glyph)
            {
                glyph = fontGeometry.getGlyph('?');
                if (!glyph)
                    continue;
            }
            double al, ab, ar, at;
            glyph->getQuadAtlasBounds(al, ab, ar, at);

            glm::vec2 texCoordMin{al, ab};
            glm::vec2 texCoordMax{ar, at};

            double pl, pb, pr, pt;
            glyph->getQuadPlaneBounds(pl, pb, pr, pt);

            glm::vec2 quadMin{pl, pb};
            glm::vec2 quadMax{pr, pt};

            quadMin *= fsScale, quadMax *= fsScale;
            quadMin += glm::vec2(x, y), quadMax += glm::vec2(x, y);

            float texelWidth = 1.0f / (float)atlasTexture.GetWidth();
            float texelHeight = 1.0f / (float)atlasTexture.GetHeight();

            texCoordMin *= glm::vec2(texelWidth, texelHeight);
            texCoordMax *= glm::vec2(texelWidth, texelHeight);

            TextInstancedData data{.TexCoord0 = texCoordMin,
                                   .TexCoord1 = {texCoordMin.x, texCoordMax.y},
                                   .TexCoord2 = texCoordMax,
                                   .TexCoord3 = {texCoordMax.x, texCoordMin.y},
                                   .PositionOffset0 = transform * glm::vec4(quadMin, 0.0f, 1.0f),
                                   .PositionOffset1 = transform * glm::vec4{quadMin.x, quadMax.y, 0.0f, 1.0f},
                                   .PositionOffset2 = transform * glm::vec4(quadMax, 0.0f, 1.0f),
                                   .PositionOffset3 = transform * glm::vec4{quadMax.x, quadMin.y, 0.0f, 1.0f},
                                   .ForegroundColor = config.ForegroundColor,
                                   .BackgroundColor = config.BackgroundColor,
                                   .EntityID = entityID};
            std::vector<byte> instancedData(sizeof(TextInstancedData));
            memcpy(instancedData.data(), &data, sizeof(TextInstancedData));
            m_Transparent.emplace_back(Entity{transform,
                                              &textModel,
                                              std::vector<BindingSet*>{m_TextBindingSet, &atlasBindingSet},
                                              std::move(instancedData)});

            if (it != end)
            {
                double advance = glyph->getAdvance();
                char32_t nextCharacter = *it;
                fontGeometry.getAdvance(advance, character, nextCharacter);

                x += fsScale * advance + config.KerningOffset;
            }
        }
    }
} // namespace BeeEngine
