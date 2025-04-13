//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#include <glm/glm.hpp>

#include "Core/String.h"
#include "Core/TypeDefines.h"
#include "TextRenderingConfiguration.h"

namespace BeeEngine
{
    class Model;
    class BindingSet;
    class Font;

    namespace Internal
    {
        class RenderingQueue;
    }
    class CommandBuffer final
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(void* handle, Internal::RenderingQueue* renderingQueue)
            : m_Handle(handle), m_RenderingQueue(renderingQueue)
        {
        }
        ~CommandBuffer() = default;
        // Mostly handled internally. Should be called before any drawing commands
        void BeginRecording();

        void DrawString(const String& text,
                        Font& font,
                        BindingSet& cameraBindingSet,
                        const glm::mat4& transform,
                        const TextRenderingConfiguration& config,
                        int32_t entityId = -1);
        void DrawRect(const glm::mat4& transform, const Color4& color, BindingSet& cameraBindingSet, float lineWidth);
        void SubmitInstance(Model& model, std::vector<BindingSet*>& bindingSets, std::span<byte> instanceData);
        void SubmitLine(const glm::vec3& start,
                        const glm::vec3& end,
                        BindingSet& cameraBindingSet,
                        const Color4& color = Color4::Black,
                        float lineWidth = 0.1f);
        void Flush();

        // Mostly handled internally. Should be called after all drawing commands
        void EndRecording();

        [[nodiscard]] bool IsValid() const { return m_Handle != nullptr && m_RenderingQueue != nullptr; }
        operator bool() { return IsValid(); }
        void* GetBufferHandle() const { return m_Handle; }
        template <typename T>
        T GetBufferHandleAs() const
        {
            return *reinterpret_cast<T const*>(&m_Handle);
        }

        void Invalidate()
        {
            m_Handle = nullptr;
            m_RenderingQueue = nullptr;
        }

    private:
        void* m_Handle = nullptr;
        Internal::RenderingQueue* m_RenderingQueue = nullptr;
    };
} // namespace BeeEngine
