//
// Created by alexl on 13.05.2023.
//

#include "DebugLayer.h"
#include "Instrumentor.h"
#include "AllocatorStatistics.h"
#include <imgui.h>


namespace BeeEngine
{
    namespace Debug
    {
        int frameCounter = 0;
        int numberOfFramesToCapture = 10;
        void DebugLayer::OnAttach()
        {

        }

        void DebugLayer::OnDetach()
        {

        }

        void DebugLayer::OnUpdate()
        {
            if(Instrumentor::Get().IsSessionActive())
            {
                frameCounter++;
                if(frameCounter >= numberOfFramesToCapture)
                {
                    frameCounter = 0;
                    BEE_DEBUG_END_PROFILING_SESSION();
                }
            }
        }
        inline double toMB(size_t bytes)
        {
            //BeeCoreTrace("Allocated memory: {0} bytes", bytes);
            double result = bytes / 1024.0 / 1024.0;
            //BeeCoreTrace("Allocated memory: {0} MB", result);
            return result;
        }
        void DebugLayer::OnGUIRendering()
        {
            ImGui::Begin("Profiler");
            ImGui::Text("Number of frames to capture");
            ImGui::InputInt("##inputframes", &numberOfFramesToCapture);
            if(ImGui::Button("Start profiling session"))
            {
                BEE_DEBUG_START_PROFILING_SESSION("BeeEngineRuntime", "runtime.json");
            }
            if(ImGui::Button("Capture 1 frame"))
            {
                numberOfFramesToCapture = 1;
                BEE_DEBUG_START_PROFILING_SESSION("BeeEngineRuntime", "runtime.json");
            }
            if(ImGui::Button("Capture 10 frames"))
            {
                numberOfFramesToCapture = 10;
                BEE_DEBUG_START_PROFILING_SESSION("BeeEngineRuntime", "runtime.json");
            }
            ImGui::End();


            ImGui::Begin("Allocator statistics");
            const BeeEngine::Internal::AllocatorStatistics& stats = BeeEngine::Internal::AllocatorStatistics::GetStatistics();
            ImGui::Text("Total allocated memory: %.10f MB", toMB(stats.totalAllocatedMemory.load()));
            ImGui::Text("Native Allocated memory: %.10f MB", toMB(stats.allocatedMemory.load()));
            ImGui::Text("GC heap size: %.10f MB", toMB(stats.gcHeapSize.load()));
            ImGui::Text("GC used memory: %.10f MB", toMB(stats.gcUsedMemory.load()));
            ImGui::Text("GC generations: %llu", stats.gcGenerations.load());
            //ImGui::Text("Allocated blocks: %llu", stats.allocatedBlocks.load());
            //ImGui::Text("Free blocks: %llu", stats.freeBlocks.load());
            //ImGui::Text("Free blocks combined: %llu", stats.blocksCombined.load());
            //ImGui::Text("Memory pages: %llu", stats.totalMemoryPages.load());
            ImGui::End();

            m_RendererStatisticsGUI.Render();
        }

        void DebugLayer::OnEvent(EventDispatcher &e)
        {

        }
    }
}
