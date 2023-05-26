//
// Created by alexl on 26.05.2023.
//

#include "EditorApplication.h"
#include "EditorLayer.h"

namespace BeeEngine::Editor
{

    EditorApplication::EditorApplication(WindowProperties&& properties) noexcept
    : Application(properties)
    {
        PushLayer(CreateRef<EditorLayer>());
    }
}
