//
// Created by alexl on 19.05.2023.
//

#include "BeeEngine.h"
#include "gtest/gtest.h"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    BeeEngine::InitEngine();
    return RUN_ALL_TESTS();
}