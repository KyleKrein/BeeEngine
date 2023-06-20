//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class GraphicsDevice
    {
    public:
        GraphicsDevice() = default;
        virtual ~GraphicsDevice() = default;

        GraphicsDevice(const GraphicsDevice&) = delete;
        GraphicsDevice& operator=(const GraphicsDevice&) = delete;

        virtual void WindowResized(uint32_t width, uint32_t height) = 0;

        /*[[nodiscard]] virtual Ref<Surface> GetSurface() const = 0;
        [[nodiscard]] virtual Ref<CommandPool> GetCommandPool() const = 0;
        [[nodiscard]] virtual DeviceID GetDeviceID() const = 0;
        [[nodiscard]] virtual Ref<GraphicsQueue> GetGraphicsQueue() const = 0;
        [[nodiscard]] virtual Ref<GraphicsQueue> GetPresentQueue() const = 0;
        [[nodiscard]] virtual Ref<SwapChain> GetSwapChain() const = 0;*/
    };
}
