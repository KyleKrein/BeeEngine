//
// Created by alexl on 17.07.2023.
//

#include "RenderingQueue.h"
#include "Core/DeletionQueue.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"
#include "Renderer.h"
#include "MSDFData.h"
#include "Core/Application.h"
#include "ext/matrix_transform.hpp"
#include "gtx/quaternion.hpp"
#include "Core/Math/Math.h"

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
        m_Statistics.TotalInstanceCount++;
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
        m_Statistics.TotalInstanceCount = 0;
        m_Statistics.TransparentInstanceCount = 0;
        m_Statistics.OpaqueInstanceCount = 0;
        m_Statistics.DrawCallCount = 0;
        m_Statistics.VertexCount = 0;
        m_Statistics.IndexCount = 0;
    }
    struct TextInstancedData
    {
        glm::vec2 TexCoord0;
        glm::vec2 TexCoord1;
        glm::vec2 TexCoord2;
        glm::vec2 TexCoord3;
        glm::vec3 PositionOffset0;
        glm::vec3 PositionOffset1;
        glm::vec3 PositionOffset2;
        glm::vec3 PositionOffset3;
        Color4 ForegroundColor;
        Color4 BackgroundColor;
    };
    void RenderingQueue::SubmitTextImpl(const std::string &text, Font &font, BindingSet& cameraBindingSet, const glm::mat4 &transform,
                                        const TextRenderingConfiguration& config)
    {
        BeeExpects(IsValidString(text));
        auto& textModel = Application::GetInstance().GetAssetManager().GetModel("Renderer_Font");

        auto& fontGeometry = font.GetMSDFData().FontGeometry;
        auto& metrics = fontGeometry.getMetrics();
        auto& atlasTexture = font.GetAtlasTexture();

        double x = 0.0;
        double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
        double y = 0.0;//-fsScale * metrics.ascenderY;

        const auto spaceGlyph =  fontGeometry.getGlyph(' ');

        UTF8StringView textView(text);
        auto it = textView.begin();
        auto end = textView.end();


        while(it != text.end())
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
            if(character == ' ')
            {
                if(it != end)
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
                if(it != end)
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
                .PositionOffset0 = transform * glm::vec4(quadMin, 0.0f, 1.0f),
                .PositionOffset1 = transform * glm::vec4{quadMin.x, quadMax.y, 0.0f, 1.0f},
                .PositionOffset2 = transform * glm::vec4(quadMax, 0.0f, 1.0f),
                .PositionOffset3 = transform * glm::vec4{quadMax.x, quadMin.y, 0.0f, 1.0f},
                .ForegroundColor = config.ForegroundColor,
                .BackgroundColor = config.BackgroundColor,
            };
            SubmitInstanceImpl({.Model = &textModel, .BindingSets = {&cameraBindingSet, atlasTexture.GetBindingSet()}}, {(byte*)&data, sizeof(TextInstancedData)});

            if(it != end)
            {
                double advance = glyph->getAdvance();
                char32_t nextCharacter = *it;
                fontGeometry.getAdvance(advance, character, nextCharacter);

                x += fsScale * advance + config.KerningOffset;
            }
        }
    }

    void RenderingQueue::SubmitLineImpl(const glm::vec3 &start, const glm::vec3 &end, const Color4 &color, BindingSet &cameraBindingSet, float lineWidth)
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

        SubmitInstanceImpl({.Model = &lineModel, .BindingSets = {&cameraBindingSet}}, {(byte*)&data, sizeof(LineInstancedData)});
    }


}
