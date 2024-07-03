//
// Created by alexl on 26.05.2023.
//

#include "EditorApplication.h"
#include "Core/Application.h"
#include "Core/Move.h"
#include "Debug/DebugLayer.h"
#include "EditorLayer.h"
#include "Locale/Locale.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine::Editor
{

    EditorApplication::EditorApplication(ConfigFile&& config) noexcept
        : Application(config.GetApplicationProperties()), m_Config(BeeMove(config))
    {
        PushOverlay(CreateRef<Debug::DebugLayer>());
        PushLayer(CreateRef<EditorLayer>(m_Config));
    }
    EditorApplication::~EditorApplication()
    {
        m_Config.X = WindowHandler::GetInstance()->GetXPosition();
        m_Config.Y = WindowHandler::GetInstance()->GetYPosition();
        m_Config.IsMaximized = IsMaximized();
        m_Config.Save(ConfigFile::DefaultPath);
    }
} // namespace BeeEngine::Editor
