//
// Created by alexl on 17.07.2023.
//

#include "RenderingQueue.h"

#include <numeric>

#include "Core/Application.h"
#include "Core/DeletionQueue.h"
#include "Core/Math/Math.h"
#include "MSDFData.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"
#include "Renderer.h"
#include "SceneRenderer.h"
#include "ext/matrix_transform.hpp"
#include "gtx/quaternion.hpp"

namespace BeeEngine::Internal
{
    RendererStatistics RenderingQueue::s_Statistics = {};
    static constexpr size_t MIN_SIZE = 1024 * 1024 * 10;

    RenderingQueue::RenderingQueue()
    {
        m_InstanceBuffers.push_back(InstancedBuffer::Create(MIN_SIZE));
        s_Statistics.AllocatedGPUMemory += MIN_SIZE;
        s_Statistics.AllocatedGPUBuffers++;
    }

    RenderingQueue::~RenderingQueue()
    {
        s_Statistics.AllocatedGPUMemory -=
            std::accumulate(m_InstanceBuffers.begin(),
                            m_InstanceBuffers.end(),
                            0,
                            [](size_t acc, const auto& buffer) { return acc + buffer->GetSize(); });
        s_Statistics.AllocatedGPUBuffers -= m_InstanceBuffers.size();
        s_Statistics.AllocatedCPUMemory -=
            std::accumulate(m_SubmittedInstances.begin(),
                            m_SubmittedInstances.end(),
                            0,
                            [](size_t acc, const auto& instance) { return acc + instance.second.Data.size(); });
    }

    RenderingQueue::RenderingQueue(size_t sizeInBytes)
    {
        m_InstanceBuffers.push_back(InstancedBuffer::Create(sizeInBytes));
        s_Statistics.AllocatedGPUMemory += sizeInBytes;
        s_Statistics.AllocatedGPUBuffers++;
    }

    void RenderingQueue::SubmitInstance(RenderInstance&& instance, gsl::span<byte> instanceData)
    {
        if (!m_SubmittedInstances.contains(instance))
        {
            m_SubmittedInstances[instance] = RenderData{};
            auto newSize = instanceData.size() * 100;
            m_SubmittedInstances[instance].Data.resize(newSize);
            s_Statistics.AllocatedCPUMemory += newSize;
        }
        auto& renderData = m_SubmittedInstances[instance];
        if (renderData.Offset + instanceData.size() > renderData.Data.size())
        {
            auto deltaSize = instanceData.size() * 100;
            renderData.Data.resize(renderData.Data.size() + deltaSize);
            s_Statistics.AllocatedCPUMemory += deltaSize;
        }
        memcpy(renderData.Data.data() + renderData.Offset, instanceData.data(), instanceData.size());
        // renderData.Data[renderData.Offset] = *instanceData.data();
        renderData.Offset += instanceData.size();
        renderData.InstanceCount++;
        s_Statistics.TotalInstanceCount++;
    }

    void RenderingQueue::Flush(CommandBuffer& commandBuffer)
    {
        for (auto& [instance, data] : m_SubmittedInstances)
        {
            if (data.InstanceCount == 0)
                continue;
            // size_t startingIndex = m_CurrentInstanceBufferIndex;
            while (data.Offset > m_InstanceBuffers[m_CurrentInstanceBufferIndex]->GetSize() ||
                   m_InstanceBuffers[m_CurrentInstanceBufferIndex]->IsSubmitted())
            {
                m_CurrentInstanceBufferIndex++;
                if (m_CurrentInstanceBufferIndex >= m_InstanceBuffers.size())
                {
                    auto newSize = std::max(data.Offset, MIN_SIZE);
                    m_InstanceBuffers.push_back(InstancedBuffer::Create(newSize));
                    s_Statistics.AllocatedGPUMemory += newSize;
                    s_Statistics.AllocatedGPUBuffers++;
                }
            }
            m_InstanceBuffers[m_CurrentInstanceBufferIndex]->SetData(data.Data.data(), data.Offset);
            Renderer::DrawInstanced(commandBuffer,
                                    *instance.Model,
                                    *m_InstanceBuffers[m_CurrentInstanceBufferIndex],
                                    instance.BindingSets,
                                    data.InstanceCount);
            s_Statistics.DrawCallCount++;
            s_Statistics.VertexCount += instance.Model->GetVertexCount() * data.InstanceCount;
            s_Statistics.IndexCount += instance.Model->GetIndexCount() * data.InstanceCount;
            m_InstanceBuffers[m_CurrentInstanceBufferIndex]->Submit();
            data.Reset();
            // m_CurrentInstanceBufferIndex++;
            m_CurrentInstanceBufferIndex = 0;
        }
        DeletionQueue::RendererFlush().Flush();
    }

    void RenderingQueue::FinishFrame(CommandBuffer& commandBuffer)
    {
        // size_t takenSize = 0;
        // while (true)
        //{
        Flush(commandBuffer);
        //}
        DeletionQueue::Frame().PushFunction(
            [this]()
            {
                for (auto& buffer : m_InstanceBuffers)
                {
                    buffer->ResetSubmition();
                }
            });
    }

    void RenderingQueue::ResetStatistics()
    {
        s_Statistics.TotalInstanceCount = 0;
        s_Statistics.TransparentInstanceCount = 0;
        s_Statistics.OpaqueInstanceCount = 0;
        s_Statistics.DrawCallCount = 0;
        s_Statistics.VertexCount = 0;
        s_Statistics.IndexCount = 0;
    }
    void RenderingQueue::SubmitText(const String& text,
                                    Font& font,
                                    BindingSet& cameraBindingSet,
                                    const glm::mat4& transform,
                                    const TextRenderingConfiguration& config,
                                    int32_t entityId)
    {
        BeeExpects(IsValidString(text));
        auto& textModel = Application::GetInstance().GetAssetManager().GetModel("Renderer_Font");

        auto& fontGeometry = font.GetMSDFData().FontGeometry;
        auto& metrics = fontGeometry.getMetrics();
        auto& atlasTexture = font.GetAtlasTexture();
        auto& atlasBindingSet = font.GetAtlasBindingSet();

        double x = 0.0;
        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0; //-fsScale * metrics.ascenderY;

        const auto* const spaceGlyph = fontGeometry.getGlyph(' ');

        UTF8StringView textView(text);
        auto it = textView.begin();
        auto end = textView.end();

        while (it != text.end())
        {
            char32_t character = *it++;

            if (character == '\r')
                continue;

            if (character == '\n')
            {
                x = 0;
                y -= fsScale * metrics.lineHeight + config.LineSpacing;
                continue;
            }
            if (character == ' ')
            {
                if (it != end)
                {
                    double advance = spaceGlyph->getAdvance();
                    char32_t nextCharacter = *it;
                    fontGeometry.getAdvance(advance, character, nextCharacter);

                    x += fsScale * advance + config.KerningOffset;
                }

                continue;
            }
            if (character == '\t')
            {
                if (it != end)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        double advance = spaceGlyph->getAdvance();
                        char32_t nextCharacter = ' ';
                        fontGeometry.getAdvance(advance, character, nextCharacter);

                        x += fsScale * advance + config.KerningOffset;
                    }
                    double advance = spaceGlyph->getAdvance();
                    char32_t nextCharacter = *it;
                    fontGeometry.getAdvance(advance, character, nextCharacter);

                    x += fsScale * advance + config.KerningOffset;
                }
                continue;
            }
            auto glyph = fontGeometry.getGlyph(character);
            if (!glyph)
            {
                glyph = fontGeometry.getGlyph('?');
                if (!glyph)
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

            TextInstancedData data{.TexCoord0 = texCoordMin,
                                   .TexCoord1 = {texCoordMin.x, texCoordMax.y},
                                   .TexCoord2 = texCoordMax,
                                   .TexCoord3 = {texCoordMax.x, texCoordMin.y},
                                   .PositionOffset0 = transform * glm::vec4(quadMin, 0.0f, 1.0f),
                                   .PositionOffset1 = transform * glm::vec4{quadMin.x, quadMax.y, 0.0f, 1.0f},
                                   .PositionOffset2 = transform * glm::vec4(quadMax, 0.0f, 1.0f),
                                   .PositionOffset3 = transform * glm::vec4{quadMax.x, quadMin.y, 0.0f, 1.0f},
                                   .ForegroundColor = config.ForegroundColor,
                                   .BackgroundColor = config.BackgroundColor,
                                   .EntityID = entityId + 1};
            SubmitInstance({.Model = &textModel, .BindingSets = {&cameraBindingSet, &atlasBindingSet}},
                           {(byte*)&data, sizeof(TextInstancedData)});

            if (it != end)
            {
                double advance = glyph->getAdvance();
                char32_t nextCharacter = *it;
                fontGeometry.getAdvance(advance, character, nextCharacter);

                x += fsScale * advance + config.KerningOffset;
            }
        }
    }

    void RenderingQueue::SubmitLine(const glm::vec3& start,
                                    const glm::vec3& end,
                                    const Color4& color,
                                    BindingSet& cameraBindingSet,
                                    float lineWidth)
    {
        auto& lineModel = Application::GetInstance().GetAssetManager().GetModel("Renderer_Line");
        struct LineInstancedData
        {
            Color4 Color;
            glm::vec3 PositionOffset0;
            glm::vec3 PositionOffset1;
            glm::vec3 PositionOffset2;
            glm::vec3 PositionOffset3;
        };
        glm::mat4 transform = Math::GetTransformFromTo(start, end, lineWidth);
        LineInstancedData data{
            .Color = color,
            .PositionOffset0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
            .PositionOffset1 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f),
            .PositionOffset2 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
            .PositionOffset3 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
        };

        SubmitInstance({.Model = &lineModel, .BindingSets = {&cameraBindingSet}},
                       {(byte*)&data, sizeof(LineInstancedData)});
    }

} // namespace BeeEngine::Internal
