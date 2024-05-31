//
// Created by alexl on 17.07.2023.
//

#include "RendererStatisticsGUI.h"

namespace BeeEngine::Internal
{
    static double ConvertFromBytesToMegabytes(const size_t& bytes)
    {
        return (double)bytes / 1024.0 / 1024.0;
    }
    void RendererStatisticsGUI::Render()
    {
        auto& stats = Renderer::GetStatistics();
        ImGui::Begin("Renderer Statistics");
        ImGui::Text("Draw calls: %zu", stats.DrawCallCount);
        ImGui::Text("Total Instance count: %zu", stats.TotalInstanceCount);
        ImGui::Text("Opaque Instances: %zu", stats.OpaqueInstanceCount);
        ImGui::Text("Transparent Instances: %zu", stats.TransparentInstanceCount);
        ImGui::Text("Vertex count: %zu", stats.VertexCount);
        ImGui::Text("Index count: %zu", stats.IndexCount);
        ImGui::Text("Allocated GPU memory: %.3f MB", ConvertFromBytesToMegabytes(stats.AllocatedGPUMemory));
        ImGui::Text("Allocated CPU memory: %.3f MB", ConvertFromBytesToMegabytes(stats.AllocatedCPUMemory));
        ImGui::Text("Allocated GPU buffers: %zu", stats.AllocatedGPUBuffers);
        ImGui::End();
    }
} // namespace BeeEngine::Internal
