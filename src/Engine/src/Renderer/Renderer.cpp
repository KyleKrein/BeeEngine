//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"
#include "RenderingQueue.h"

namespace BeeEngine
{
    RenderAPI Renderer::s_Api = RenderAPI::NotAvailable;
    Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Color4 Renderer::s_ClearColor = Color4::CornflowerBlue;
    RendererStatistics Renderer::s_Statistics {};
    bool Renderer::s_FrameStarted = false;

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
}
