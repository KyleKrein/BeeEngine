//
// Created by alexl on 27.01.2023.
//

#include "Renderer.h"


namespace BeeEngine
{
    RenderAPI Renderer::s_Api = RenderAPI::NotAvailable;
    Ref<RendererAPI> Renderer::s_RendererAPI = nullptr;
    Color4 Renderer::s_ClearColor = Color4::DarkGray;
}
