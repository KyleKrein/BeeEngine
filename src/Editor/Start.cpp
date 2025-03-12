//
// Created by alexl on 26.05.2023.
//

#include "BeeEngine.h"
#include "Core/Move.h"
#include "src/ConfigFile.h"
#include "src/EditorApplication.h"

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    Editor::ConfigFile config = Editor::ConfigFile::Load(Editor::ConfigFile::DefaultPath());
    return new BeeEngine::Editor::EditorApplication(BeeMove(config));
}
