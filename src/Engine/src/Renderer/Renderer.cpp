//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"
#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"

namespace BeeEngine
{
    RenderAPI Renderer::s_Api = RenderAPI::NotAvailable;
    Scope<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Color4 Renderer::s_ClearColor = Color4::DarkGray;
    RendererStatistics Renderer::s_Statistics {};

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
        //Internal::RenderingQueue::Initialize();
    }
}
