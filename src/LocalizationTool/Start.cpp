//
// Created by alexl on 19.10.2023.
//
#include <BeeEngine.h>
#include "LocalizationToolApplication.h"

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    return new BeeEngine::LocalizationTool::LocalizationToolApplication({1280, 720, "BeeEngine Localization Tool", VSync::On});
}