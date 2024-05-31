//
// Created by alexl on 17.10.2023.
//

#include <Core/Hash.h>
#include <Core/Reflection.h>
#include <gtest/gtest.h>
using namespace BeeEngine;

TEST(HashTest, TestStringHashing)
{
    UTF8String my_string = "Hello, world!";
    uint64_t seed = 42;
    uint64_t hash1 = Hash(my_string, seed);
    uint64_t hash2 = Hash(my_string, seed);
    ASSERT_EQ(hash1, hash2); // проверим, что хеши одинаковые
}

TEST(HashTest, TestPrimitiveTypeHashing)
{
    int my_int = 123;
    uint64_t seed = 42;
    uint64_t hash1 = Hash(my_int, seed);
    uint64_t hash2 = Hash(my_int, seed);
    ASSERT_EQ(hash1, hash2); // проверим, что хеши одинаковые
}

TEST(HashTest, TestRangeHashing)
{
    std::vector<int> my_vec = {1, 2, 3, 4, 5};
    uint64_t seed = 42;
    uint64_t hash1 = Hash(my_vec, seed);
    uint64_t hash2 = Hash(my_vec, seed);
    ASSERT_EQ(hash1, hash2); // проверим, что хеши одинаковые
}

TEST(HashTest, DifferentStrings)
{
    UTF8String string1 = "Hello, world!";
    UTF8String string2 = "world! Hello,";
    uint64_t seed = 42;
    ASSERT_NE(Hash(string1, seed), Hash(string2, seed)); // хеши должны быть разными
}

TEST(HashTest, SameStringsDifferentSeeds)
{
    UTF8String my_string = "Hello, world!";
    uint64_t seed1 = 42;
    uint64_t seed2 = 43;
    ASSERT_NE(Hash(my_string, seed1), Hash(my_string, seed2)); // хеши должны быть разными
}

TEST(HashTest, TestDataChange)
{
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {1, 2, 3, 4};
    uint64_t seed = 42;
    ASSERT_NE(Hash(vec1, seed), Hash(vec2, seed)); // хеши должны быть разными
}

TEST(HashTest, TestPrimitiveDataChange)
{
    int int1 = 42;
    int int2 = 43;
    uint64_t seed = 42;
    ASSERT_NE(Hash(int1, seed), Hash(int2, seed)); // хеши должны быть разными
}

TEST(HashTest, TestEmptyAndNonEmptyString)
{
    UTF8String string1 = "";
    UTF8String string2 = "Not empty";
    uint64_t seed = 42;
    ASSERT_NE(Hash(string1, seed), Hash(string2, seed)); // хеши должны быть разными
}

struct TestStruct
{
    int a;
    double b;
    float c;
    REFLECT()
};

REFLECT_STRUCT_BEGIN(TestStruct)
REFLECT_STRUCT_MEMBER(a)
REFLECT_STRUCT_MEMBER(b)
REFLECT_STRUCT_MEMBER(c)
REFLECT_STRUCT_END()

TEST(HashTest, TestReflectableStruct)
{
    TestStruct s1 = {1, 2.0, 3.0f};
    TestStruct s2 = {1, 2.0, 3.0f};
    TestStruct s3 = {1, 2.0, 4.0f};
    uint64_t seed = 42;
    ASSERT_EQ(Reflection::Hash(s1, seed), Reflection::Hash(s2, seed)); // хеши должны быть одинаковыми
    ASSERT_NE(Reflection::Hash(s1, seed), Reflection::Hash(s3, seed)); // хеши должны быть разными
}

struct TestStructForReflection
{
    int a;
    UTF8String b;
    REFLECT()
};
REFLECT_STRUCT_BEGIN(TestStructForReflection)
REFLECT_STRUCT_MEMBER(a)
REFLECT_STRUCT_MEMBER(b)
REFLECT_STRUCT_END()

TEST(HashTest, TestReflectableStructWithStrings)
{
    TestStructForReflection s1 = {1, "Hello"};
    TestStructForReflection s2 = {1, "Hello"};
    TestStructForReflection s3 = {1, "Pochemu"};
    uint64_t seed = 42;
    ASSERT_NE(Reflection::Hash(s1, seed),
              Reflection::Hash(s2, seed)); // хеши должны быть разными, потому что String - сложный тип, использующий
                                           // динамическую память. с ним Reflection::Hash работает неправильно
    ASSERT_NE(Reflection::Hash(s1, seed), Reflection::Hash(s3, seed)); // хеши должны быть разными
}

struct TestStructWithHashing
{
    int a;
    double b;
    float c;
    UTF8String d;
    uint64_t Hash(uint64_t seed = 0) const
    {
        uint64_t hash = seed;
        hash = BeeEngine::Hash(a, hash);
        hash = BeeEngine::Hash(b, hash);
        hash = BeeEngine::Hash(c, hash);
        hash = BeeEngine::Hash(d, hash);
        return hash;
    }
};

TEST(HashTest, TestStructWithHashing)
{
    TestStructWithHashing s1 = {1, 2.0, 3.0f, "Hello"};
    TestStructWithHashing s2 = {1, 2.0, 3.0f, "Hello"};
    TestStructWithHashing s3 = {1, 2.0, 4.0f, "Hello"};
    uint64_t seed = 42;
    ASSERT_EQ(Hash(s1, seed), Hash(s2, seed)); // хеши должны быть одинаковыми
    ASSERT_NE(Hash(s1, seed), Hash(s3, seed)); // хеши должны быть разными
}