//
// Created by alexl on 21.05.2023.
//

#pragma once

#include "BeeEngine.h"

class TestApplication : public BeeEngine::Application
{
public:
    TestApplication(BeeEngine::WindowProperties& properties)
    : BeeEngine::Application(properties) {}
    ~TestApplication() override {};
};