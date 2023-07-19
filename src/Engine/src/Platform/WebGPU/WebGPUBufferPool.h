//
// Created by alexl on 17.07.2023.
//

#pragma once
#include <webgpu/webgpu.h>
#include <vector>
namespace BeeEngine::Internal
{
    enum class WebGPUBufferUsage
    {
        Vertex
    };
    struct WebGPUBuffer
    {
        WGPUBuffer Buffer;
        uint32_t Size;
        uint32_t Offset;
        WebGPUBufferUsage Usage;

        bool operator==(const WebGPUBuffer& other) const
        {
            return Buffer == other.Buffer && Size == other.Size && Offset == other.Offset && Usage == other.Usage;
        }
        bool operator!=(const WebGPUBuffer& other) const
        {
            return !(*this == other);
        }
    private:
        WebGPUBuffer(WGPUBuffer buffer, uint32_t size, uint32_t offset, WebGPUBufferUsage usage, bool isDestroyed)
        : Buffer(buffer), Size(size), Offset(offset), Usage(usage), m_IsDestroyed(isDestroyed)
        {

        }
        bool m_IsDestroyed;
        friend class WebGPUBufferPool;
    };
    class WebGPUBufferPool final
    {
    public:
        WebGPUBufferPool();
        ~WebGPUBufferPool();
        WebGPUBufferPool(const WebGPUBufferPool&) = delete;
        WebGPUBufferPool(WebGPUBufferPool&&) = delete;
        WebGPUBufferPool& operator=(const WebGPUBufferPool&) = delete;
        WebGPUBufferPool& operator=(WebGPUBufferPool&&) = delete;
        [[nodiscard("To avoid gpu memory leaks")]]
        const WebGPUBuffer& RequestBuffer(uint32_t size, WebGPUBufferUsage usage);

        void ReleaseBuffer(const WebGPUBuffer& buffer);
    private:
        WGPUBuffer CreateBuffer(uint32_t size, WebGPUBufferUsage usage);
        void DestroyBuffer(const WebGPUBuffer& buffer);
        std::vector<WebGPUBuffer> m_Buffers;
    };
}
