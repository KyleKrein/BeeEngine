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

    void Renderer::DrawString(const String &text, Font &font, BindingSet& cameraBindingSet, const glm::mat4 &transform, const Color4 &foregroundColor,
                              const Color4 &backgroundColor)
    {
        Internal::RenderingQueue::SubmitText(text, font, cameraBindingSet, transform, foregroundColor, backgroundColor);
    }
}
