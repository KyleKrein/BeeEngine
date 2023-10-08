//
// Created by alexl on 08.10.2023.
//

#include <gtest/gtest.h>
#include "BeeEngine.h"
using namespace BeeEngine;
TEST(TestTypeDefines, TestToStringForInt)
{
    int testValue = 42;
    UTF8String result = ToString(testValue);
    EXPECT_EQ(result, "42");
}

TEST(TestTypeDefines, TestToStringForBool)
{
    bool testValue = true;
    UTF8String result = ToString(testValue);
    EXPECT_EQ(result, "true");
}

TEST(TestTypeDefines, TestTypeNameForInt)
{
    int testValue = 0;
    UTF8String result = TypeName(testValue);
    EXPECT_EQ(result, "int");  // Это может зависеть от вашей системы и компилятора
}

TEST(TestTypeDefines, TestTypeNameForFloat)
{
    float testValue = 0.0f;
    UTF8String result = TypeName(testValue);
    EXPECT_EQ(result, "float");  // Это может зависеть от вашей системы и компилятора
}
struct TestStruct {
    int x;
    TestStruct(int val) : x(val) {}
};

TEST(TestTypeDefines, TestTypeNameForCustomStruct)
{
    TestStruct testValue(0);
    UTF8String result = TypeName(testValue);
    EXPECT_EQ(result, "TestStruct");
}

// Примеры структур для тестирования CreateScope и CreateRef


TEST(TestTypeDefines, TestCreateFrameScope)
{
    auto frameScopeObj = BeeEngine::CreateFrameScope<TestStruct>(5);
    EXPECT_EQ(frameScopeObj->x, 5);
}

TEST(TestTypeDefines, TestCreateScope)
{
    auto scopeObj = BeeEngine::CreateScope<TestStruct>(10);
    EXPECT_EQ(scopeObj->x, 10);
}

TEST(TestTypeDefines, TestCreateRef)
{
    auto refObj = BeeEngine::CreateRef<TestStruct>(15);
    EXPECT_EQ(refObj->x, 15);
}

// Enum для тестирования
enum class TestEnum {
    VALUE_ONE,
    VALUE_TWO
};

TEST(TestTypeDefines, TestEnumToString)
{
    TestEnum testEnum = TestEnum::VALUE_ONE;
    std::string result = BeeEngine::EnumToString(testEnum);
    EXPECT_EQ(result, "VALUE_ONE");  // Это может зависеть от вашей реализации EnumToString
}

TEST(TestTypeDefines, TestToStringForString)
{
    std::string testValue = "TestString";
    std::string result = BeeEngine::ToString(testValue);
    EXPECT_EQ(result, "TestString");
}

TEST(TestTypeDefines, TestToStringForVector)
{
    std::vector<int> testVector = {1, 2, 3};
    std::string result = BeeEngine::ToString(testVector);
    EXPECT_EQ(result, "[1, 2, 3]");
}

TEST(TestTypeDefines, TestToStringForCustomTypeWithToString)
{
    struct TestStruct {
        int x;
        TestStruct(int val) : x(val) {}
        UTF8String ToString() const {
            return "TestStruct ToString";
        }
    };
    TestStruct testValue(0);
    UTF8String result = BeeEngine::ToString(testValue);
    EXPECT_EQ(result, "TestStruct ToString");
}
