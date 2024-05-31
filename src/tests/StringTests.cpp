//
// Created by alexl on 21.05.2023.
//

#include "Core/ResourceManager.h"
#include "gtest/gtest.h"
using namespace BeeEngine;

TEST(ResourceManagerTest, ProcessFilePathTest)
{
    BeeEngine::String filepath = "C:\\Users\\alexl\\Desktop\\BeeEngine\\src\\tests\\StringTests.cpp";
    BeeEngine::String result = BeeEngine::ResourceManager::ProcessFilePath(std::string_view(filepath));
    ASSERT_EQ(result, "C:/Users/alexl/Desktop/BeeEngine/src/tests/StringTests.cpp");
}

TEST(ResourceManagerTest, GetNameFromFilePathTest)
{
    BeeEngine::String filepath = "C:\\Users\\alexl\\Desktop\\BeeEngine\\src\\tests\\StringTests.cpp";
    BeeEngine::String result = BeeEngine::ResourceManager::GetNameFromFilePath(std::string_view(filepath));
    ASSERT_EQ(result, "StringTests");
}

TEST(PathTest, DefaultConstructor)
{
    Path p;
    EXPECT_TRUE(p.IsEmpty());
}

TEST(PathTest, StringConstructor)
{
    Path p("some/path");
    EXPECT_EQ(p.AsUTF8(), "some/path");
}

TEST(PathTest, IsAbsolute_IsRelative)
{
#if defined(WINDOWS)
    Path p1("C:/absolute/path");
#else
    Path p1("/absolute/path");
#endif
    Path p2("relative/path");
    EXPECT_TRUE(p1.IsAbsolute());
    EXPECT_FALSE(p1.IsRelative());
    EXPECT_FALSE(p2.IsAbsolute());
    EXPECT_TRUE(p2.IsRelative());
}

TEST(PathTest, GetParent)
{
    Path p("/parent/child");
    Path parent = p.GetParent();
    EXPECT_EQ(parent.AsUTF8(), "/parent");
}

TEST(PathTest, GetFileName)
{
    Path p("/parent/child.txt");
    Path filename = p.GetFileName();
    EXPECT_EQ(filename.AsUTF8(), "child.txt");
}

TEST(PathTest, ReplaceExtension)
{
    Path p("/parent/child.txt");
    p.ReplaceExtension(".md");
    EXPECT_EQ(p.AsUTF8(), "/parent/child.md");
}

TEST(PathTest, GetFileNameWithoutExtension)
{
    Path p("/parent/child.txt");
    Path filename = p.GetFileNameWithoutExtension();
    EXPECT_EQ(filename.AsUTF8(), "child");
}

TEST(PathTest, GetExtension)
{
    Path p("/parent/child.txt");
    Path extension = p.GetExtension();
    EXPECT_EQ(extension.AsUTF8(), ".txt");
}

TEST(PathTest, GetRelativePath)
{
    Path p1("/parent/child");
    Path p2("/parent");
    Path relative = p1.GetRelativePath(p2);
    EXPECT_EQ(relative.AsUTF8(), "child");
}

TEST(PathTest, GetAbsolutePath)
{
    Path p("child");
    Path absolute = p.GetAbsolutePath(); // Предположим, что текущий каталог — /parent
    EXPECT_EQ(absolute.AsUTF8(), Path(std::filesystem::current_path() / "child").AsUTF8());
}

TEST(PathTest, OperatorOverload)
{
    Path p1("/first/path");
    Path p2("/second/path");
    EXPECT_FALSE(p1 == p2);
    EXPECT_TRUE(p1 != p2);
}

TEST(PathTest, OperatorAssignment)
{
    Path p;
    p = "new/path";
    EXPECT_EQ(p.AsUTF8(), "new/path");
}

TEST(PathTest, Clear)
{
    Path p("some/path");
    p.Clear();
    EXPECT_TRUE(p.IsEmpty());
}

TEST(PathTest, OperatorSlash)
{
    Path p1("/parent");
    Path p2("child");
    Path result = p1 / p2;
    EXPECT_EQ(result.AsUTF8(), "/parent/child");
}

TEST(PathTest, ReplaceFileName)
{
    Path p("/parent/child.txt");
    p.ReplaceFileName("newchild.txt");
    EXPECT_EQ(p.AsUTF8(), "/parent/newchild.txt");
}

TEST(PathTest, RemoveExtension)
{
    Path p("/parent/child.txt");
    Path result = p.RemoveExtension();
    EXPECT_EQ(result.AsUTF8(), "/parent/child");
}

TEST(PathTest, RemoveFileName)
{
    Path p("/parent/child.txt");
    Path result = p.RemoveFileName();
    EXPECT_EQ(result.AsUTF8(), "/parent");
}

TEST(PathTest, EmptyStringConstructor)
{
    Path p("");
    EXPECT_TRUE(p.IsEmpty());
}

TEST(PathTest, NullptrConstructor)
{
    Path p(nullptr);
    EXPECT_TRUE(p.IsEmpty());
}

TEST(PathTest, PathSeparators)
{
    Path p1("some/path");
    Path p2("some\\path");
    EXPECT_EQ(p1.AsUTF8(), p2.AsUTF8());
}

// Комбинированный тест: конвертация туда и обратно
TEST(StringTest, ConvertUTF8ToUTF16ToUTF8)
{
    UTF8String original = "Original String";
    UTF16String converted = ConvertUTF8ToUTF16(original);
    UTF8String back = ConvertUTF16ToUTF8(converted);
    EXPECT_EQ(original, back);
}

TEST(SplitTest, NormalCase)
{
    auto result = SplitString("one::two::three", "::");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
}

TEST(SplitTest, EmptyString)
{
    auto result = SplitString("", "::");
    ASSERT_TRUE(result.empty());
}

TEST(SplitTest, OnlyDelimiters)
{
    auto result = SplitString("::", "::");
    ASSERT_EQ(result.size(), 0);
}

TEST(SplitTest, LeadingDelimiters)
{
    auto result = SplitString("::one::two::three", "::");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
}

TEST(SplitTest, TrailingDelimiters)
{
    auto result = SplitString("one::two::three::", "::");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
}

enum class TestEnum
{
    VALUE_ONE,
    VALUE_TWO,
    VALUE_THREE
};

enum class UnhandledEnum
{
    VAL1 = 500,
    VAL2 = 600
};

TEST(EnumTest, EnumToStringHandled)
{
    EXPECT_EQ(EnumToString(TestEnum::VALUE_ONE), "VALUE_ONE");
    EXPECT_EQ(EnumToString(TestEnum::VALUE_TWO), "VALUE_TWO");
}

/*TEST(EnumTest, EnumToStringUnhandled) {
    // Пример предполагает, что UnhandledEnum не обрабатывается magic_enum
    EXPECT_EQ(EnumToString<UnhandledEnum>((UnhandledEnum)500), "UnhandledEnum::500");
}*///THIS DOES NOT COMPILE

TEST(EnumTest, StringToEnumHandled)
{
    EXPECT_EQ(StringToEnum<TestEnum>("VALUE_ONE"), TestEnum::VALUE_ONE);
    EXPECT_EQ(StringToEnum<TestEnum>("VALUE_TWO"), TestEnum::VALUE_TWO);
}

/*TEST(EnumTest, StringToEnumUnhandled) {
    // Пример предполагает, что UnhandledEnum не обрабатывается magic_enum
    EXPECT_EQ(StringToEnum<UnhandledEnum>("UnhandledEnum::500"), UnhandledEnum::VAL1);
}*/

TEST(EnumTest, RoundTrip)
{
    auto original = TestEnum::VALUE_ONE;
    auto str = EnumToString(original);
    auto back = StringToEnum<TestEnum>(str);
    EXPECT_EQ(original, back);
}