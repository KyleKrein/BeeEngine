//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"
#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"

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

    void Renderer::SetAPI(const RenderAPI& api)
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(s_Api == RenderAPI::NotAvailable, "Can't change Renderer API after initialization!");
        s_Api = api;
        BeeCoreInfo("Using {} Renderer API", ToString(api));
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
        Internal::RenderingQueue::Initialize();
    }

    void Renderer::DrawString(const String &text, Font &font, BindingSet& cameraBindingSet, const glm::mat4 &transform, const TextRenderingConfiguration& config)
    {
        Internal::RenderingQueue::SubmitText(text, font, cameraBindingSet, transform, config);
    }

    void Renderer::SubmitLine(const glm::vec3 &start, const glm::vec3 &end,
                              BindingSet &cameraBindingSet, const Color4 &color, float lineWidth)
    {
        Internal::RenderingQueue::SubmitLine(start, end, color, cameraBindingSet, lineWidth);
    }

    void Renderer::DrawRect(const glm::mat4 &transform, const Color4 &color, BindingSet &cameraBindingSet, float lineWidth)
    {
        static constexpr std::array<glm::vec4, 4> QuadVertexPositions = {
                glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
                glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
                glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
                glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)
        };
        glm::vec3 lineVertices[4];
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * QuadVertexPositions[i];
        SubmitLine(lineVertices[0], lineVertices[1], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[1], lineVertices[2], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[2], lineVertices[3], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[3], lineVertices[0], cameraBindingSet, color, lineWidth);
    }
}
