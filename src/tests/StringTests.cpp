//
// Created by alexl on 21.05.2023.
//

#include "Core/ResourceManager.h"
#include "gtest/gtest.h"

TEST(ResourceManagerTest, ProcessFilePathTest)
{
    BeeEngine::String filepath = "C:\\Users\\alexl\\Desktop\\BeeEngine\\src\\tests\\StringTests.cpp";
    BeeEngine::String result = BeeEngine::ResourceManager::ProcessFilePath(filepath);
    ASSERT_EQ(result, "C:/Users/alexl/Desktop/BeeEngine/src/tests/StringTests.cpp");
}

TEST(ResourceManagerTest, GetNameFromFilePathTest)
{
    BeeEngine::String filepath = "C:\\Users\\alexl\\Desktop\\BeeEngine\\src\\tests\\StringTests.cpp";
    BeeEngine::String result = BeeEngine::ResourceManager::GetNameFromFilePath(filepath);
    ASSERT_EQ(result, "StringTests");
}