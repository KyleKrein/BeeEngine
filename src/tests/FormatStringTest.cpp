//
// Created by alexl on 08.10.2023.
//

#include <gtest/gtest.h>
#include <BeeEngine.h>
TEST(FormatStringTest, HandlesUnindexedArguments) {
    auto result = BeeEngine::FormatString("Hello, {}!", "World");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(FormatStringTest, HandlesIndexedArguments) {
    auto result = BeeEngine::FormatString("{1}, {0}!", "World", "Hello");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(FormatStringTest, HandlesMixedArguments) {
    auto result = BeeEngine::FormatString("{1}, {}!", "World", "Hello");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(FormatStringTest, HandlesInsufficientArguments) {
    auto result = BeeEngine::FormatString("{1}, {}!", "only_one_arg");
    EXPECT_EQ(result, "{1}, only_one_arg!");
}

TEST(FormatStringTest, HandlesNoArguments) {
    auto result = BeeEngine::FormatString("Hello, World!", "ignored_arg");
    EXPECT_EQ(result, "Hello, World!");
}

TEST(FormatStringTest, HandlesMultipleArguments) {
    auto result = BeeEngine::FormatString("Sum of {} and {} is {}", 1, 2, 3);
    EXPECT_EQ(result, "Sum of 1 and 2 is 3");
}

TEST(FormatStringTest, HandlesOnlyBraces) {
    auto result = BeeEngine::FormatString("{}{}{}", 1, 2, 3);
    EXPECT_EQ(result, "123");
}

TEST(FormatStringTest, HandlesEmptyBraces) {
    auto result = BeeEngine::FormatString("{}", "");
    EXPECT_EQ(result, "");
}

TEST(FormatStringTest, HandlesNoBraces) {
    auto result = BeeEngine::FormatString("No braces here", "ignored_arg");
    EXPECT_EQ(result, "No braces here");
}

TEST(FormatStringTest, HandlesMultipleSameIndex) {
    auto result = BeeEngine::FormatString("{0}{0}{0}", "ha");
    EXPECT_EQ(result, "hahaha");
}

TEST(FormatStringTest, HandlesInvalidIndex) {
    auto result = BeeEngine::FormatString("{not_a_number}", "ignored_arg");
    EXPECT_EQ(result, "{not_a_number}");
}

TEST(FormatStringTest, HandlesOutOfBoundsIndex) {
    auto result = BeeEngine::FormatString("{3}", 1, 2, 3);
    EXPECT_EQ(result, "{3}");
}

TEST(FormatStringTest, HandlesNegativeIndex) {
    auto result = BeeEngine::FormatString("{-1}", "ignored_arg");
    EXPECT_EQ(result, "{-1}");
}

TEST(FormatStringTest, HandlesSpecialCharacters) {
    auto result = BeeEngine::FormatString("Special characters: \\ { }", "ignored_arg");
    EXPECT_EQ(result, "Special characters: \\ { }");
}

TEST(FormatStringTest, HandlesUnmatchedBraces) {
    auto result = BeeEngine::FormatString("{unmatched", "ignored_arg");
    EXPECT_EQ(result, "{unmatched");
}
TEST(FormatStringTest, HandlesOnlyNumbers) {
    auto result = BeeEngine::FormatString("123456", "ignored_arg");
    EXPECT_EQ(result, "123456");
}

TEST(FormatStringTest, HandlesNullCharacter) {
    auto result = BeeEngine::FormatString("This is a null character: \0", "ignored_arg");
    EXPECT_EQ(result, "This is a null character: ");
}

TEST(FormatStringTest, HandlesMultipleArgumentTypes) {
    auto result = BeeEngine::FormatString("{} {} {}", 1, "two", 3.0);
    EXPECT_EQ(result, "1 two 3.000000");
}

TEST(FormatStringTest, HandlesNestedBraces) {
    auto result = BeeEngine::FormatString("{{}}", "ignored_arg");
    EXPECT_EQ(result, "{{}}");
}

TEST(FormatStringTest, HandlesMissingClosingBrace) {
    auto result = BeeEngine::FormatString("{0", "ignored_arg");
    EXPECT_EQ(result, "{0");
}

TEST(FormatStringTest, HandlesMissingOpeningBrace) {
    auto result = BeeEngine::FormatString("0}", "ignored_arg");
    EXPECT_EQ(result, "0}");
}

TEST(FormatStringTest, HandlesArgumentCountMismatch) {
    auto result = BeeEngine::FormatString("{} {} {}", 1, 2);
    EXPECT_EQ(result, "1 2 {}");
}

TEST(FormatStringTest, HandlesEmptyFormatString) {
    auto result = BeeEngine::FormatString("", "ignored_arg");
    EXPECT_EQ(result, "");
}