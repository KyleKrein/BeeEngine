//
// Created by alexl on 26.05.2023.
//

#include "BeeEngine.h"
#include "src/ConfigFile.h"
#include "src/EditorApplication.h"

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    return new BeeEngine::Editor::EditorApplication(BeeEngine::Editor::ConfigFile::LoadWindowConfiguration());
}