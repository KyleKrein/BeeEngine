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

        void SetWorkingDirectory(const std::filesystem::path& path) noexcept
        {
            m_WorkingDirectory = path;
        }
    private:
        Ref<Scene> m_Context;

        std::filesystem::path m_WorkingDirectory;

        void DrawComponents(Entity entity);

        template<typename T, typename UIFunction>
        void DrawComponentUI(std::string_view label, Entity entity, UIFunction func);
        template<typename T, typename UIFunction>
        void DrawConsistentComponentUI(std::string_view label, Entity entity, UIFunction func);

        template<typename T>
        void AddComponentPopup(std::string_view label, Entity entity);
        void DrawVec3ComponentUI(std::string_view label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
    };
}
