//
// Created by Aleksandr on 09.03.2024.
//

#pragma once
#include "VulkanBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "Renderer/InstancedBuffer.h"

namespace BeeEngine::Internal
{

    class VulkanInstancedBuffer final: public InstancedBuffer
    {
    public:
        VulkanInstancedBuffer(size_t size);
        ~VulkanInstancedBuffer() override;

        void SetData(void* data, size_t size) override;

        void Bind(CommandBuffer& cmd) override;

        size_t GetSize() override;
    private:
        VulkanGraphicsDevice& m_GraphicsDevice;
        size_t m_Size;
        VulkanBuffer m_Buffer;
    };

}
