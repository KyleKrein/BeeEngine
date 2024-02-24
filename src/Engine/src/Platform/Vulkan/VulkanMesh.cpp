//
// Created by Aleksandr on 24.02.2024.
//

#include "VulkanMesh.h"

namespace BeeEngine::Internal
{
    VulkanMesh::~VulkanMesh()
    {
        vmaDestroyBuffer(GetVulkanAllocator(), m_VertexBuffer.Buffer, m_VertexBuffer.Memory);
        if(VulkanMesh::IsIndexed())
        {
            vmaDestroyBuffer(GetVulkanAllocator(), m_IndexBuffer.Buffer, m_IndexBuffer.Memory);
        }
        vmaDestroyBuffer(GetVulkanAllocator(), m_AccelerationStructure.Buffer.Buffer, m_AccelerationStructure.Buffer.Memory);
        m_Device.GetDevice().destroyAccelerationStructureKHR(m_AccelerationStructure.AccelerationStructure);
    }

    uint32_t VulkanMesh::GetVertexCount() const
    {
        return m_VertexCount;
    }

    uint32_t VulkanMesh::GetIndexCount() const
    {
        return m_IndexCount;
    }

    void VulkanMesh::Bind(void* commandBuffer)
    {
        auto cmd = *reinterpret_cast<vk::CommandBuffer*>(&commandBuffer);
        vk::Buffer vertexBuffers[] = {m_VertexBuffer.Buffer};
        vk::DeviceSize offsets[] = {0};
        cmd.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        if(IsIndexed())
        {
            cmd.bindIndexBuffer(m_IndexBuffer.Buffer, 0, vk::IndexType::eUint32);
        }
    }

    bool VulkanMesh::IsIndexed() const
    {
        return m_IndexBuffer.Buffer != nullptr;
    }

    VulkanMesh::VulkanMesh(const std::vector<Vertex>& vertices)
        : m_Device(VulkanGraphicsDevice::GetInstance()), m_VertexCount(vertices.size()), m_IndexCount(0)
    {
        CreateVertexBuffer(vertices);
        CreateAccelerationStructure(sizeof(Vertex));
    }

    VulkanMesh::VulkanMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_Device(VulkanGraphicsDevice::GetInstance()), m_VertexCount(vertices.size()), m_IndexCount(indices.size())
    {
        CreateVertexBuffer(vertices);
        CreateIndexBuffer(indices);
        CreateAccelerationStructure(sizeof(Vertex));
    }

    VulkanMesh::VulkanMesh(void* verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t>& indices)
    : m_Device(VulkanGraphicsDevice::GetInstance()), m_VertexCount(vertexCount), m_IndexCount(indices.size())
    {
        CreateVertexBuffer(verticesData, size, vertexCount);
        CreateIndexBuffer(indices);
        CreateAccelerationStructure(size/vertexCount);
    }

    void VulkanMesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
    {
        CreateVertexBuffer(const_cast<void*>(static_cast<const void*>(vertices.data())), vertices.size() * sizeof(Vertex), vertices.size());
    }

    void VulkanMesh::CreateVertexBuffer(void* verticesData, size_t size, size_t vertexCount)
    {
        m_VertexBuffer = m_Device.CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR, vk::MemoryPropertyFlagBits::eDeviceLocal);
        VulkanBuffer bufferForMapping = m_Device.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void* mappedData = nullptr;
        vmaMapMemory(GetVulkanAllocator(), bufferForMapping.Memory, &mappedData);
        memcpy(mappedData, verticesData, size);
        vmaUnmapMemory(GetVulkanAllocator(), bufferForMapping.Memory);
        m_Device.CopyBuffer(bufferForMapping.Buffer, m_VertexBuffer.Buffer, size);
        vmaDestroyBuffer(GetVulkanAllocator(), bufferForMapping.Buffer, bufferForMapping.Memory);
    }

    void VulkanMesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        auto size = indices.size() * sizeof(uint32_t);
        auto indicesData = const_cast<void*>(static_cast<const void*>(indices.data()));

        m_IndexBuffer = m_Device.CreateBuffer(size, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR, vk::MemoryPropertyFlagBits::eDeviceLocal);
        VulkanBuffer bufferForMapping = m_Device.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        void* mappedData = nullptr;
        vmaMapMemory(GetVulkanAllocator(), bufferForMapping.Memory, &mappedData);
        memcpy(mappedData, indicesData, size);
        vmaUnmapMemory(GetVulkanAllocator(), bufferForMapping.Memory);
        m_Device.CopyBuffer(bufferForMapping.Buffer, m_IndexBuffer.Buffer, size);
        vmaDestroyBuffer(GetVulkanAllocator(), bufferForMapping.Buffer, bufferForMapping.Memory);
    }

    void VulkanMesh::CreateAccelerationStructure(size_t vertexStride)
    {
        auto device = m_Device.GetDevice();
        vk::BufferDeviceAddressInfo vertexBufferAddressInfo = {};
        vertexBufferAddressInfo.sType = vk::StructureType::eBufferDeviceAddressInfo;
        vertexBufferAddressInfo.buffer = m_VertexBuffer.Buffer;
        auto vertexBufferAddress = device.getBufferAddress(&vertexBufferAddressInfo);
        vk::BufferDeviceAddressInfo indexBufferAddressInfo = {};
        indexBufferAddressInfo.sType = vk::StructureType::eBufferDeviceAddressInfo;
        indexBufferAddressInfo.buffer = m_IndexBuffer.Buffer;
        auto indexBufferAddress = device.getBufferAddress(&indexBufferAddressInfo);

        vk::AccelerationStructureGeometryTrianglesDataKHR triangles = {};
        triangles.sType = vk::StructureType::eAccelerationStructureGeometryTrianglesDataKHR;
        triangles.vertexFormat = vk::Format::eR32G32B32Sfloat;
        triangles.vertexData.deviceAddress = vertexBufferAddress;
        triangles.vertexStride = vertexStride;
        triangles.indexType = vk::IndexType::eUint32;
        triangles.indexData.deviceAddress = indexBufferAddress;
        triangles.maxVertex = m_VertexCount;

        vk::AccelerationStructureGeometryKHR geometry = {};
        geometry.sType = vk::StructureType::eAccelerationStructureGeometryKHR;
        geometry.geometryType = vk::GeometryTypeKHR::eTriangles;
        geometry.geometry.triangles = triangles;
        geometry.flags = vk::GeometryFlagBitsKHR::eOpaque;

        vk::AccelerationStructureBuildGeometryInfoKHR buildInfo = {};
        buildInfo.sType = vk::StructureType::eAccelerationStructureBuildGeometryInfoKHR;
        buildInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
        buildInfo.flags = vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace;
        buildInfo.setGeometries(geometry);

        uint32_t primitiveCount = m_IndexCount / 3;
        vk::AccelerationStructureBuildSizesInfoKHR buildSizesInfo = device.getAccelerationStructureBuildSizesKHR(
            vk::AccelerationStructureBuildTypeKHR::eDevice, buildInfo, primitiveCount);

        m_AccelerationStructure.Buffer = m_Device.CreateBuffer(buildSizesInfo.accelerationStructureSize, vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR /*| vk::BufferUsageFlagBits::eShaderDeviceAddress*/, vk::MemoryPropertyFlagBits::eDeviceLocal);

        vk::AccelerationStructureCreateInfoKHR createInfo = {};
        createInfo.sType = vk::StructureType::eAccelerationStructureCreateInfoKHR;
        createInfo.buffer = m_AccelerationStructure.Buffer.Buffer;
        createInfo.setSize(buildSizesInfo.accelerationStructureSize);
        createInfo.setType(vk::AccelerationStructureTypeKHR::eBottomLevel);
        m_AccelerationStructure.AccelerationStructure = device.createAccelerationStructureKHR(createInfo);

        VulkanBuffer scratchBuffer = m_Device.CreateBuffer(buildSizesInfo.buildScratchSize, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = m_Device.GetCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer;
        device.allocateCommandBuffers(&allocInfo, &commandBuffer);

        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        commandBuffer.begin(&beginInfo);

        buildInfo.setMode(vk::BuildAccelerationStructureModeKHR::eBuild);
        buildInfo.dstAccelerationStructure = m_AccelerationStructure.AccelerationStructure;
        buildInfo.scratchData.deviceAddress = device.getBufferAddress({scratchBuffer.Buffer});

        vk::AccelerationStructureBuildRangeInfoKHR buildRangeInfo{};
        buildRangeInfo.primitiveCount = primitiveCount;
        buildRangeInfo.primitiveOffset = 0;
        buildRangeInfo.firstVertex = 0;
        buildRangeInfo.transformOffset = 0;

        commandBuffer.buildAccelerationStructuresKHR(buildInfo, &buildRangeInfo);

        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        auto graphicsQueue = m_Device.GetGraphicsQueue();

        graphicsQueue.submit(1, &submitInfo, vk::Fence(nullptr));
        graphicsQueue.waitIdle();

        device.freeCommandBuffers(m_Device.GetCommandPool(), commandBuffer);

        vmaDestroyBuffer(GetVulkanAllocator(), scratchBuffer.Buffer, scratchBuffer.Memory);
    }
}
