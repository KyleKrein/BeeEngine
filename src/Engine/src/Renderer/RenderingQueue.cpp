//
// Created by alexl on 17.07.2023.
//

#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"
#include "Renderer.h"


namespace BeeEngine::Internal
{
    static constexpr size_t MIN_SIZE = 1024 * 1024 * 10;

    void RenderingQueue::Initialize()
    {
        RenderingQueue* self = &GetInstance();
        DeletionQueue::Main().PushFunction([self]()
        {
            self->m_InstanceBuffers.clear();
            self->m_SubmittedInstances.clear();
        });
        self->m_InstanceBuffers.push_back(InstancedBuffer::Create(MIN_SIZE));
        self->m_Statistics.AllocatedGPUMemory+= MIN_SIZE;
        self->m_Statistics.AllocatedGPUBuffers++;
    }

    void RenderingQueue::SubmitInstanceImpl(RenderInstance &&instance, gsl::span<byte> instanceData)
    {
        if(!m_SubmittedInstances.contains(instance))
        {
            m_SubmittedInstances[instance] = RenderData{};
            auto newSize = instanceData.size() * 100;
            m_SubmittedInstances[instance].Data.resize(newSize);
            m_Statistics.AllocatedCPUMemory += newSize;
        }
        auto& renderData = m_SubmittedInstances[instance];
        if(renderData.Offset + instanceData.size() > renderData.Data.size())
        {
            auto deltaSize = instanceData.size() * 100;
            renderData.Data.resize(renderData.Data.size() + deltaSize);
            m_Statistics.AllocatedCPUMemory += deltaSize;
        }
        memcpy(renderData.Data.data() + renderData.Offset, instanceData.data(), instanceData.size());
        //renderData.Data[renderData.Offset] = *instanceData.data();
        renderData.Offset += instanceData.size();
        renderData.InstanceCount++;
        m_Statistics.InstanceCount++;
    }

    void RenderingQueue::FlushImpl()
    {
        for (auto& [instance, data] : m_SubmittedInstances)
        {
            if(data.InstanceCount == 0)
                continue;
            //size_t startingIndex = m_CurrentInstanceBufferIndex;
            while(data.Offset > m_InstanceBuffers[m_CurrentInstanceBufferIndex]->GetSize() || m_InstanceBuffers[m_CurrentInstanceBufferIndex]->IsSubmitted())
            {
                m_CurrentInstanceBufferIndex++;
                if (m_CurrentInstanceBufferIndex >= m_InstanceBuffers.size())
                {
                    auto newSize = std::max(data.Offset, MIN_SIZE);
                    m_InstanceBuffers.push_back(InstancedBuffer::Create(newSize));
                    m_Statistics.AllocatedGPUMemory += newSize;
                    m_Statistics.AllocatedGPUBuffers++;
                }
            }
            m_InstanceBuffers[m_CurrentInstanceBufferIndex]->SetData(data.Data.data(), data.Offset);
            Renderer::DrawInstanced(*instance.Model, *m_InstanceBuffers[m_CurrentInstanceBufferIndex], instance.BindingSets, data.InstanceCount);
            m_Statistics.DrawCallCount++;
            m_Statistics.VertexCount += instance.Model->GetVertexCount() * data.InstanceCount;
            m_Statistics.IndexCount += instance.Model->GetIndexCount() * data.InstanceCount;
            m_InstanceBuffers[m_CurrentInstanceBufferIndex]->Submit();
            data.Reset();
            //m_CurrentInstanceBufferIndex++;
            m_CurrentInstanceBufferIndex = 0;
        }
        DeletionQueue::RendererFlush().Flush();
    }

    void RenderingQueue::FinishFrameImpl()
    {
        //size_t takenSize = 0;
        //while (true)
        //{
        FlushImpl();
        //}
        DeletionQueue::Frame().PushFunction([this]()
        {
            for (auto& buffer : m_InstanceBuffers)
            {
                buffer->ResetSubmition();
            }
        });
    }

    void RenderingQueue::ResetStatisticsImpl()
    {
        m_Statistics.InstanceCount = 0;
        m_Statistics.DrawCallCount = 0;
        m_Statistics.VertexCount = 0;
        m_Statistics.IndexCount = 0;
    }
}
