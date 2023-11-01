//
// Created by alexl on 17.07.2023.
//

#pragma once

#include <gsl/span>
#include "Core/TypeDefines.h"
#include "Model.h"
#include "Platform/WebGPU/WebGPUBufferPool.h"
#include "RendererStatistics.h"
#include "Core/Color4.h"
#include "Font.h"
#include "TextRenderingConfiguration.h"

namespace BeeEngine::Internal
{
    struct RenderInstance
    {
        Model *Model;
        std::vector<BindingSet *> BindingSets;

        bool operator==(const RenderInstance& other) const
        {
            return Model == other.Model && BindingSets == other.BindingSets;
        }
        bool operator!=(const RenderInstance& other) const
        {
            return !(*this == other);
        }
    };
    struct RenderData
    {
        std::vector<byte> Data {};
        size_t Offset {0};
        size_t InstanceCount {0};

        void Reset()
        {
            Offset = 0;
            InstanceCount = 0;
        }
    };
}
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
            auto hashResult = (std::hash<BeeEngine::Model *>()(k.Model));
            for (auto& bindingSet : k.BindingSets)
            {
                hashResult ^= (std::hash<BeeEngine::BindingSet *>()(bindingSet) << 1);
            }
            return hashResult >> 1;
        }
    };
}
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
        static void Initialize();
        static void SubmitInstance(RenderInstance&& instance, gsl::span<byte> instanceData)
        {
            GetInstance().SubmitInstanceImpl(std::move(instance), instanceData);
        }

        static void SubmitText(const std::string& text, Font& font, BindingSet& cameraBindingSet, const glm::mat4& transform, const TextRenderingConfiguration& config)
        {
            GetInstance().SubmitTextImpl(text, font, cameraBindingSet, transform, config);
        }

        static void SubmitLine(const glm::vec3& start, const glm::vec3& end, const Color4& color, BindingSet& cameraBindingSet, float lineWidth)
        {
            GetInstance().SubmitLineImpl(start, end, color, cameraBindingSet, lineWidth);
        }

        static void Flush()
        {
            GetInstance().FlushImpl();
        }
        static void FinishFrame()
        {
            GetInstance().FinishFrameImpl();
        }
        static const RendererStatistics& GetStatistics()
        {
            return GetInstance().m_Statistics;
        }
        static void ResetStatistics()
        {
            GetInstance().ResetStatisticsImpl();
        }
    private:

        void SubmitTextImpl(const std::string &text, Font& font, BindingSet& cameraBindingSet, const glm::mat4 &transform, const TextRenderingConfiguration& config);
        static RenderingQueue& GetInstance()
        {
            static RenderingQueue instance {};
            return instance;
        }
        void SubmitInstanceImpl(RenderInstance&& instance, gsl::span<byte> instanceData);
        void SubmitLineImpl(const glm::vec3& start, const glm::vec3& end, const Color4& color, BindingSet& cameraBindingSet, float lineWidth);
        void FlushImpl();
        void FinishFrameImpl();

        std::unordered_map<RenderInstance, RenderData> m_SubmittedInstances;
        std::vector<Scope<InstancedBuffer>> m_InstanceBuffers;
        size_t m_CurrentInstanceBufferIndex {0};
        RendererStatistics m_Statistics {};
        void ResetStatisticsImpl();
    };
}
