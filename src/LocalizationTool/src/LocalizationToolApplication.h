//
// Created by alexl on 19.10.2023.
//

#pragma once
#include <BeeEngine.h>
#include "LocalizationToolLayer.h"

namespace BeeEngine::LocalizationTool
{
    class LocalizationToolApplication : public Application
    {
    public:
        LocalizationToolApplication(ApplicationProperties&& properties) noexcept
        : Application(properties)
        {
            PushLayer(CreateRef<LocalizationToolLayer>());
        }
    };
}