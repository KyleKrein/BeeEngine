//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUTestLayer.h"

void WebGPUTestLayer::OnAttach()
{

}

void WebGPUTestLayer::OnDetach()
{

}

void WebGPUTestLayer::OnUpdate()
{

}

void WebGPUTestLayer::OnGUIRendering()
{
    m_FpsCounter.Update();
    m_FpsCounter.Render();
}
