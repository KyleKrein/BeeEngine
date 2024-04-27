//
// Created by alexl on 08.06.2023.
//

#pragma once

enum RenderAPI
{
    NotAvailable = 0,
    /*Metal = 2,
    DirectX = 3,*/
    WebGPU = 1,
    Vulkan = 2,
};

namespace BeeEngine
{
    RenderAPI GetPrefferedRenderAPI();
}