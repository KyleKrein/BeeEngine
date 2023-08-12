//
// Created by alexl on 17.07.2023.
//

#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"
#include "Renderer.h"
#include "MSDFData.h"
#include "Core/Application.h"

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
    struct TextInstancedData
    {
        alignas(alignof(glm::vec3)) glm::vec2 TexCoord0;
        alignas(alignof(glm::vec3)) glm::vec2 TexCoord1;
        alignas(alignof(glm::vec3)) glm::vec2 TexCoord2;
        alignas(alignof(glm::vec3)) glm::vec2 TexCoord3;
        alignas(alignof(glm::vec3)) glm::vec2 PositionOffset0;
        alignas(alignof(glm::vec3)) glm::vec2 PositionOffset1;
        alignas(alignof(glm::vec3)) glm::vec2 PositionOffset2;
        alignas(alignof(glm::vec3)) glm::vec2 PositionOffset3;
        alignas(alignof(glm::vec4)) Color4 ForegroundColor;
        alignas(alignof(glm::vec4)) Color4 BackgroundColor;
        alignas(alignof(glm::vec4)) glm::mat4 Transform;
    };
    void RenderingQueue::SubmitTextImpl(const std::string &text, Font &font, BindingSet& cameraBindingSet, const glm::mat4 &transform,
                                        const Color4 &foregroundColor, const Color4 &backgroundColor)
    {
        auto& textModel = Application::GetInstance().GetAssetManager().GetModel("Renderer_Font");

        auto& fontGeometry = font.GetMSDFData().FontGeometry;
        auto& metrics = fontGeometry.getMetrics();
        auto& atlasTexture = font.GetAtlasTexture();

        double x = 0.0;
        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0;//-fsScale * metrics.ascenderY;
        float lineHeightOffset = 0.0f;

        size_t length = text.size();

        for (size_t i = 0; i < length; ++i)
        {
            char32_t character = text[i];

            if (character == '\r')
                continue;

            if (character == '\n')
            {
                x = 0;
                y -= fsScale * metrics.lineHeight + lineHeightOffset;
                continue;
            }
            if (character == '\t')
                character = ' ';
            auto glyph = fontGeometry.getGlyph(character);
            if(!glyph)
            {
                glyph = fontGeometry.getGlyph('?');
                if(!glyph)
                    continue;
            }
            double al, ab, ar, at;
            glyph->getQuadAtlasBounds(al, ab, ar, at);

            glm::vec2 texCoordMin{al, ab};
            glm::vec2 texCoordMax{ar, at};

            double pl, pb, pr, pt;
            glyph->getQuadPlaneBounds(pl, pb, pr, pt);

            glm::vec2 quadMin{pl, pb};
            glm::vec2 quadMax{pr, pt};


            quadMin *= fsScale, quadMax *= fsScale;
            quadMin += glm::vec2(x, y), quadMax += glm::vec2(x, y);

            float texelWidth = 1.0f / (float)atlasTexture.GetWidth();
            float texelHeight = 1.0f / (float)atlasTexture.GetHeight();

            texCoordMin *= glm::vec2(texelWidth, texelHeight);
            texCoordMax *= glm::vec2(texelWidth, texelHeight);

            TextInstancedData data{
                .TexCoord0 = texCoordMin,
                .TexCoord1 = {texCoordMin.x, texCoordMax.y},
                .TexCoord2 = texCoordMax,
                .TexCoord3 = {texCoordMax.x, texCoordMin.y},
                .PositionOffset0 = quadMin,
                .PositionOffset1 = {quadMin.x, quadMax.y},
                .PositionOffset2 = quadMax,
                .PositionOffset3 = {quadMax.x, quadMin.y},
                .ForegroundColor = foregroundColor,
                .BackgroundColor = backgroundColor,
                .Transform = transform
            };
            SubmitInstanceImpl({.Model = &textModel, .BindingSets = {&cameraBindingSet, atlasTexture.GetBindingSet()}}, {(byte*)&data, sizeof(TextInstancedData)});

            if(i < length - 1)
            {
                double advance = glyph->getAdvance();
                char32_t nextCharacter = text[i + 1];
                fontGeometry.getAdvance(advance, character, nextCharacter);

                float kerningOffset = 0.0f;
                x += fsScale * advance + kerningOffset;
            }
        }
    }
}
