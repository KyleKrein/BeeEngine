//
// Created by alexl on 13.05.2023.
//

#include "DebugLayer.h"
#include "Instrumentor.h"


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
            const AllocatorStatistics& stats = GeneralPurposeAllocator::GetStatistics();
            ImGui::Text("Allocated memory: %.10f MB", toMB(stats.allocatedMemory));
            ImGui::Text("Allocated blocks: %d", stats.allocatedBlocks);
            ImGui::Text("Free blocks: %d", stats.freeBlocks);
            ImGui::Text("Free blocks combined: %d", stats.blocksCombined);
            ImGui::Text("Memory pages: %d", stats.totalMemoryPages);
            ImGui::End();
        }

        void DebugLayer::OnEvent(EventDispatcher &e)
        {

        }
    }
}
