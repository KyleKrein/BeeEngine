//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "BeeEngine.h"

namespace BeeEngine::Editor
{
    class InspectorPanel
    {
    public:
        InspectorPanel() = default;
        explicit InspectorPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);

        void OnGUIRender(Entity selectedEntity) noexcept;
    private:
        Ref<Scene> m_Context;

        void DrawComponents(Entity entity);
        void DrawVec3ComponentUI(std::string_view label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    };
}
