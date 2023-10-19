//
// Created by alexl on 26.05.2023.
//

#include "EditorApplication.h"
#include "EditorLayer.h"
#include "Debug/DebugLayer.h"
#include "Locale/Locale.h"

namespace BeeEngine::Editor
{

    EditorApplication::EditorApplication(WindowProperties&& properties) noexcept
    : Application(properties)
    {
        PushOverlay(CreateRef<Debug::DebugLayer>());
        PushLayer(CreateRef<EditorLayer>());
    }
}
