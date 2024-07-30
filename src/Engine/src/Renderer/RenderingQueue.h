//
// Created by alexl on 17.07.2023.
//

#pragma once

#include "Core/Color4.h"
#include "Core/TypeDefines.h"
#include "Font.h"
#include "Model.h"
#include "RendererStatistics.h"
#include "TextRenderingConfiguration.h"
#include <gsl/span>

namespace BeeEngine::Internal
{
    struct RenderInstance
    {
        Model* Model;
        std::vector<BindingSet*> BindingSets;

        bool operator==(const RenderInstance& other) const
        {
            return Model == other.Model && BindingSets == other.BindingSets;
        }
        bool operator!=(const RenderInstance& other) const { return !(*this == other); }
    };
    struct RenderData
    {
        std::vector<byte> Data{};
        size_t Offset{0};
        size_t InstanceCount{0};

        void Reset()
        {
            Offset = 0;
            InstanceCount = 0;
        }
    };
} // namespace BeeEngine::Internal
namespace std
{
    template <>
    struct std::hash<BeeEngine::Internal::RenderInstance>
    {
        std::size_t operator()(const BeeEngine::Internal::RenderInstance& k) const
        {
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:
            auto hashResult = (std::hash<BeeEngine::Model*>()(k.Model));
            for (auto& bindingSet : k.BindingSets)
            {
                hashResult ^= (std::hash<BeeEngine::BindingSet*>()(bindingSet) << 1);
            }
            return hashResult >> 1;
        }
    };
} // namespace std
namespace BeeEngine
{
    class SceneRenderer;
}
namespace BeeEngine::Internal
{
    class RenderingQueue final
    {
        friend BeeEngine::SceneRenderer;

    public:
        RenderingQueue();
        RenderingQueue(size_t sizeInBytes);
        ~RenderingQueue();
        void SubmitInstance(RenderInstance&& instance, gsl::span<byte> instanceData);
        void SubmitLine(const glm::vec3& start,
                        const glm::vec3& end,
                        const Color4& color,
                        BindingSet& cameraBindingSet,
                        float lineWidth);
        void SubmitText(const String& text,
                        Font& font,
                        BindingSet& cameraBindingSet,
                        const glm::mat4& transform,
                        const TextRenderingConfiguration& config,
                        int32_t entityId = -1);
        void Flush(CommandBuffer& commandBuffer);
        void FinishFrame(CommandBuffer& commandBuffer);

        static const RendererStatistics& GetGlobalStatistics() { return s_Statistics; }

        static void ResetStatistics();

    private:
        std::unordered_map<RenderInstance, RenderData> m_SubmittedInstances;
        std::vector<Scope<InstancedBuffer>> m_InstanceBuffers;
        size_t m_CurrentInstanceBufferIndex{0};
        static RendererStatistics s_Statistics;
    };
} // namespace BeeEngine::Internal
