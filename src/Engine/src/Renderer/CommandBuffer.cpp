//
// Created by Aleksandr on 18.03.2024.
//
#include "CommandBuffer.h"
#include "RenderingQueue.h"
namespace BeeEngine
{
    void CommandBuffer::BeginRecording()
    {
        BeeExpects(IsValid());
    }

    void CommandBuffer::DrawString(const String& text, Font& font, BindingSet& cameraBindingSet,
        const glm::mat4& transform, const TextRenderingConfiguration& config)
    {
        BeeExpects(IsValid());
        m_RenderingQueue->SubmitText(text, font, cameraBindingSet, transform, config);
    }

    void CommandBuffer::DrawRect(const glm::mat4& transform, const Color4& color, BindingSet& cameraBindingSet,
        float lineWidth)
    {
        BeeExpects(IsValid());
        static constexpr std::array<glm::vec4, 4> QuadVertexPositions = {
            glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
            glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
            glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
            glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)
        };
        glm::vec3 lineVertices[4];
        for (size_t i = 0; i < 4; i++)
            lineVertices[i] = transform * QuadVertexPositions[i];
        SubmitLine(lineVertices[0], lineVertices[1], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[1], lineVertices[2], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[2], lineVertices[3], cameraBindingSet, color, lineWidth);
        SubmitLine(lineVertices[3], lineVertices[0], cameraBindingSet, color, lineWidth);
    }

    void CommandBuffer::SubmitInstance(Model& model, std::vector<BindingSet*>& bindingSets,
        gsl::span<byte> instanceData)
    {
        BeeExpects(IsValid());
        m_RenderingQueue->SubmitInstance({&model, bindingSets}, instanceData);
    }

    void CommandBuffer::SubmitLine(const glm::vec3& start, const glm::vec3& end, BindingSet& cameraBindingSet,
        const Color4& color, float lineWidth)
    {
        BeeExpects(IsValid());
        m_RenderingQueue->SubmitLine(start, end, color, cameraBindingSet, lineWidth);
    }

    void CommandBuffer::Flush()
    {
        BeeExpects(IsValid());
        m_RenderingQueue->Flush(*this);
    }

    void CommandBuffer::EndRecording()
    {
        BeeExpects(IsValid());
        m_RenderingQueue->FinishFrame(*this);
    }
}
