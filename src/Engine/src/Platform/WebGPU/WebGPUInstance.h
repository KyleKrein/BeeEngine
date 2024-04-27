//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "Renderer/Instance.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUInstance: public Instance
    {
    public:
        WebGPUInstance();
        ~WebGPUInstance() override;

        [[nodiscard]] WGPUInstance GetHandle() const
        {
            return m_Instance;
        }

    private:
        WGPUInstance m_Instance;
    };
}
#endif