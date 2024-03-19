//
// Created by alexl on 27.01.2023.
//

#pragma once
#include "RenderAPI.h"
#include "Core/Logging/Log.h"
#include "RendererAPI.h"
#include "Core/Color4.h"
#include "Debug/Instrumentor.h"
#include "Core/CodeSafety/Expects.h"
#include "CommandBuffer.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Model.h"
#include "RendererStatistics.h"
#include "Font.h"

namespace BeeEngine
{
    class FrameData
    {
    public:
        FrameData(CommandBuffer commandBuffer, RendererStatistics& statistics)
            : m_MainCommandBuffer(commandBuffer), m_InProgress(true), m_Statistics(statistics)
        {}
        CommandBuffer& GetMainCommandBuffer()
        {
            return m_MainCommandBuffer;
        }
        bool IsInProgress() const
        {
            return m_InProgress;
        }

        void SetDeltaTime(Time::secondsD deltaTime)
        {
            BeeExpects(m_DeltaTime == Time::secondsD(0.0));
            m_DeltaTime = deltaTime;
        }
        [[nodiscard]] Time::secondsD GetDeltaTime() const
        {
            return m_DeltaTime;
        }
    private:
        void End()
        {
            m_InProgress = false;
        }
        friend class Renderer;
    private:
        CommandBuffer m_MainCommandBuffer;
        RendererStatistics m_Statistics;
        bool m_InProgress = false;
        Time::secondsD m_DeltaTime = Time::secondsD(0.0);
    };
    class Renderer
    {
    public:
        static RenderAPI GetAPI()
        {
            return s_Api;
        }
        static void SetAPI(const RenderAPI& api);

        static const Color4& GetClearColor()
        {
            return s_ClearColor;
        }
        static void DrawInstanced(CommandBuffer& commandBuffer, Model& model, InstancedBuffer& instancedBuffer, const std::vector<BindingSet*>& bindingSets, uint32_t instanceCount)
        {
            BEE_PROFILE_FUNCTION();
            s_RendererAPI->DrawInstanced(commandBuffer, model, instancedBuffer, bindingSets, instanceCount);
        }
        static void SubmitCommandBuffer(const CommandBuffer& commandBuffer)
        {
            s_RendererAPI->SubmitCommandBuffer(commandBuffer);
        }

        //static void DrawInstanced(const Ref<Model>& model, const Ref<UniformBuffer>& instanceBuffer, uint32_t instanceCount)
        //{
        //    BEE_PROFILE_FUNCTION();
        //    s_RendererAPI->DrawInstanced(model, instanceBuffer, instanceCount);
        //}

        static FrameData BeginFrame()
        {
            BEE_PROFILE_FUNCTION();
            return {s_RendererAPI->BeginFrame(), s_Statistics};
        }

        static void EndFrame(FrameData& frameData);

        static void StartMainCommandBuffer(FrameData& frameData)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(frameData.IsInProgress());
            s_RendererAPI->StartMainCommandBuffer(frameData.GetMainCommandBuffer());
        }

        static void EndMainCommandBuffer(FrameData& frameData)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(frameData.IsInProgress());
            s_RendererAPI->EndMainCommandBuffer(frameData.GetMainCommandBuffer());
        }

        /*static void SubmitCommandBuffers(CommandBuffer* commandBuffers, uint32_t numberOfBuffers)
        {
            BEE_PROFILE_FUNCTION();
            static GraphicsDevice& graphicsDevice = WindowHandler::GetInstance()->GetGraphicsDevice();
            graphicsDevice.SubmitCommandBuffers(commandBuffers, numberOfBuffers);
        }*/

        static const RendererStatistics& GetStatistics()
        {
            return s_Statistics;
        }

        /*static CommandBuffer GetMainCommandBuffer()
        {
            return s_RendererAPI->GetCurrentCommandBuffer();
        }*/

        static void Shutdown()
        {
            s_RendererAPI.reset();
        }

    private:

        static RenderAPI s_Api;
        static Scope<RendererAPI> s_RendererAPI;
        static Color4 s_ClearColor;
        static RendererStatistics s_Statistics;
    };
}
