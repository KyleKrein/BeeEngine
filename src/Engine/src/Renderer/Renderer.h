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
#include "RenderPass.h"
#include "Model.h"
#include "RendererStatistics.h"
#include "Font.h"
#include "TextRenderingConfiguration.h"

namespace BeeEngine
{
    class Renderer
    {
    public:
        static RenderAPI GetAPI()
        {
            return s_Api;
        }
        static void SetAPI(const RenderAPI& api)
        {
            BEE_PROFILE_FUNCTION();
            BeeCoreAssert(s_Api == RenderAPI::NotAvailable, "Can't change Renderer API after initialization!");
            s_Api = api;
            BeeCoreInfo("Using {} Renderer API", ToString(api));
            s_RendererAPI = RendererAPI::Create();
            s_RendererAPI->Init();
        }
        static void SetClearColor(const Color4& color)
        {
            BEE_PROFILE_FUNCTION();
            s_ClearColor = color;
            s_RendererAPI->SetClearColor(color);
        }
        static const Color4& GetClearColor()
        {
            return s_ClearColor;
        }
        static void Clear()
        {
            BEE_PROFILE_FUNCTION();
            s_RendererAPI->Clear();
        }
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(width > 0 && height > 0);
            s_RendererAPI->SetViewport(x, y, width, height);
        }
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
        {
            BEE_PROFILE_FUNCTION();
            vertexArray->Bind();
            s_RendererAPI->DrawIndexed(vertexArray, indexCount);
        }
        static void DrawInstanced(Model& model, InstancedBuffer& instancedBuffer, const std::vector<BindingSet*>& bindingSets, uint32_t instanceCount)
        {
            BEE_PROFILE_FUNCTION();
            s_RendererAPI->DrawInstanced(model, instancedBuffer, bindingSets, instanceCount);
        }
        static void DrawString(const String& text, Font& font, BindingSet& cameraBindingSet, const glm::mat4& transform, const TextRenderingConfiguration& config);
        static void SubmitCommandBuffer(const CommandBuffer& commandBuffer)
        {
            s_RendererAPI->SubmitCommandBuffer(commandBuffer);
        }

        static void SubmitInstance(Model& model, std::vector<BindingSet*>& bindingSets, gsl::span<byte> instanceData);
        static void Flush();
        //static void DrawInstanced(const Ref<Model>& model, const Ref<UniformBuffer>& instanceBuffer, uint32_t instanceCount)
        //{
        //    BEE_PROFILE_FUNCTION();
        //    s_RendererAPI->DrawInstanced(model, instanceBuffer, instanceCount);
        //}
        static Color4 ReadPixel(uint32_t x, uint32_t y)
        {
            BEE_PROFILE_FUNCTION();
            return s_RendererAPI->ReadPixel(x, y);
        }

        static CommandBuffer BeginFrame()
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(!s_FrameStarted);
            s_FrameStarted = true;
            return s_RendererAPI->BeginFrame();
        }

        static void EndFrame();

        static void StartMainRenderPass(in<CommandBuffer> commandBuffer)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(s_FrameStarted);
            s_RendererAPI->StartMainRenderPass(commandBuffer);
        }

        static void EndMainRenderPass(in<CommandBuffer> commandBuffer)
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(s_FrameStarted);
            s_RendererAPI->EndMainRenderPass(commandBuffer);
        }

        /*static void SubmitCommandBuffers(CommandBuffer* commandBuffers, uint32_t numberOfBuffers)
        {
            BEE_PROFILE_FUNCTION();
            static GraphicsDevice& graphicsDevice = WindowHandler::GetInstance()->GetGraphicsDevice();
            graphicsDevice.SubmitCommandBuffers(commandBuffers, numberOfBuffers);
        }*/

        static RenderPass GetCurrentRenderPass()
        {
            return s_NotMainRenderPass ? s_CurrentRenderPass : GetMainRenderPass();
        }

        static const RendererStatistics& GetStatistics()
        {
            return s_Statistics;
        }

        static void SetCurrentRenderPass(const RenderPass& pass);
        static void ResetCurrentRenderPass()
        {
            BeeExpects(s_NotMainRenderPass);
            s_NotMainRenderPass = false;
        }

        static void FinalFlush();

        static CommandBuffer GetMainCommandBuffer()
        {
            return s_RendererAPI->GetCurrentCommandBuffer();
        }

    private:
        static RenderPass GetMainRenderPass()
        {
            return s_RendererAPI->GetMainRenderPass();
        }

        static RenderAPI s_Api;
        static Ref<RendererAPI> s_RendererAPI;
        static Color4 s_ClearColor;
        static bool s_FrameStarted;
        static RendererStatistics s_Statistics;

        static bool s_NotMainRenderPass;
        static RenderPass s_CurrentRenderPass;
    };
}
