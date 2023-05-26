//
// Created by alexl on 26.05.2023.
//

#include "BeeEngine.h"
#include "src/EditorApplication.h"
#include "src/ConfigFile.h"



gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication()
{
    return new BeeEngine::Editor::EditorApplication(BeeEngine::Editor::ConfigFile::LoadWindowConfiguration());
}