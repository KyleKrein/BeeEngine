//
// Created by alexl on 19.05.2023.
//

#include "BeeEngine.h"
#include "gtest/gtest.h"
#include "ApplicationInit.h"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    BeeEngine::InitEngine();
    auto windowProperties = BeeEngine::WindowProperties(1280, 720, "BeeEngineTests");
    auto app = TestApplication(windowProperties);
    return RUN_ALL_TESTS();
}