//
// Created by alexl on 13.05.2023.
//

#include "DebugLayer.h"


namespace BeeEngine
{

    void DebugLayer::OnAttach()
    {

    }

    void DebugLayer::OnDetach()
    {

    }

    void DebugLayer::OnUpdate()
    {

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
        ImGui::Begin("Debug Settings");
        //ImGui::Text("Allocated memory: %.10f MB", toMB(MemoryProfiler::GetAllocatedSize()));
        ImGui::End();
    }

    void DebugLayer::OnEvent(EventDispatcher &e)
    {

    }
}
