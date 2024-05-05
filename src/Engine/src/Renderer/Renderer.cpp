//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"
#include "Core/DeletionQueue.h"
#include "RenderingQueue.h"

namespace BeeEngine
{
    RenderAPI Renderer::s_Api = RenderAPI::NotAvailable;
    Scope<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Color4 Renderer::s_ClearColor = Color4::DarkGray;
    RendererStatistics Renderer::s_Statistics{};

    void FrameData::CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex)
    {
        BEE_PROFILE_FUNCTION();
        Renderer::s_RendererAPI->CopyFrameBufferImageToSwapchain(framebuffer, attachmentIndex);
    }

    void Renderer::EndFrame(FrameData& frameData)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(frameData.IsInProgress());
        s_RendererAPI->EndFrame();
        frameData.End();

        s_Statistics = Internal::RenderingQueue::GetGlobalStatistics();
        Internal::RenderingQueue::ResetStatistics();
    }

    void Renderer::SetAPI(const RenderAPI& api)
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(s_Api == RenderAPI::NotAvailable, "Can't change Renderer API after initialization!");
        s_Api = api;
        BeeCoreInfo("Using {} Renderer API", ToString(api));
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
        // Internal::RenderingQueue::Initialize();
    }
} // namespace BeeEngine
