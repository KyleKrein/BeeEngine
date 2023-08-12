//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"
#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"
#include "MSDFData.h"

namespace BeeEngine
{
    RenderAPI Renderer::s_Api = RenderAPI::NotAvailable;
    Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Color4 Renderer::s_ClearColor = Color4::DarkGray;
    RendererStatistics Renderer::s_Statistics {};
    bool Renderer::s_FrameStarted = false;

    bool Renderer::s_NotMainRenderPass = false;
    RenderPass Renderer::s_CurrentRenderPass = {};

    void Renderer::SubmitInstance(Model &model, std::vector<BindingSet *> &bindingSets, gsl::span<byte> instanceData)
    {
        Internal::RenderingQueue::SubmitInstance({&model, bindingSets}, instanceData);
    }

    void Renderer::Flush()
    {
        Internal::RenderingQueue::Flush();
    }

    void Renderer::EndFrame()
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(s_FrameStarted);
        s_RendererAPI->EndFrame();
        s_FrameStarted = false;

        s_Statistics = Internal::RenderingQueue::GetStatistics();
        Internal::RenderingQueue::ResetStatistics();
    }

    void Renderer::SetCurrentRenderPass(const RenderPass& pass)
    {
        BeeExpects(!s_NotMainRenderPass);
        s_NotMainRenderPass = true;
        s_CurrentRenderPass = pass;
    }

    void Renderer::FinalFlush()
    {
        Internal::RenderingQueue::FinishFrame();
    }

    void Renderer::DrawString(const String &text, Font &font, const glm::mat4 &transform, const Color4 &color)
    {
        auto& msdfData = font.GetMSDFData();
        auto& fontGeometry = msdfData.FontGeometry;
        auto& metrics = fontGeometry.getMetrics();
        auto& atlasTexture = font.GetAtlasTexture();

        double x = 0.0;
        double fxScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0;//-fxScale * metrics.ascenderY;

        for (char32_t character : text)
        {
            auto glyph = fontGeometry.getGlyph(character);
            if(!glyph)
            {
                glyph = fontGeometry.getGlyph('?');
                if(!glyph)
                    continue;
            }
            double pl, pb, pr, pt;
            glyph->getQuadPlaneBounds(pl, pb, pr, pt);

            glm::vec2 quadMin{pl, pb};
            glm::vec2 quadMax{pr, pt};

            double al, ab, ar, at;
            glyph->getQuadAtlasBounds(al, ab, ar, at);

            glm::vec2 texCoordMin{al, ab};
            glm::vec2 texCoordMax{ar, at};

            quadMin *= fxScale, quadMax *= fxScale;
            quadMin += glm::vec2(x, y), quadMax += glm::vec2(x, y);

            float texelWidth = 1.0f / (float)atlasTexture.GetWidth();
            float texelHeight = 1.0f / (float)atlasTexture.GetHeight();

            texCoordMin *= glm::vec2(texelWidth, texelHeight);
            texCoordMax *= glm::vec2(texelWidth, texelHeight);
        }

    }
}
