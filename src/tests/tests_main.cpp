//
// Created by alexl on 19.05.2023.
//

#include "BeeEngine.h"
#include "gtest/gtest.h"
#include "ApplicationInit.h"
using namespace BeeEngine;
gsl::not_null<Application*> BeeEngine::CreateApplication(const ApplicationArgs& args)
{
    auto windowProperties = BeeEngine::WindowProperties{1280, 720, "BeeEngineTests", VSync::On};
    int argc = args.GetArgc();
    testing::InitGoogleTest(&argc, args.GetArgv());
    auto* app = new TestApplication(windowProperties);
    auto result = RUN_ALL_TESTS();
    BeeInfo("Tests finished with code: {}", result);
    app->Close();
    return app;
}

/*
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    BeeEngine::Internal::InitEngine();
    auto windowProperties = BeeEngine::WindowProperties{1280, 720, "BeeEngineTests", VSync::On};
    auto app = TestApplication(windowProperties);
    auto result = RUN_ALL_TESTS();
    BeeEngine::Internal::ShutDownEngine();
    return result;
}*/
