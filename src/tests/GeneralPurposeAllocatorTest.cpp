//
// Created by alexl on 19.05.2023.
//
#include "BeeEngine.h"
#include "gtest/gtest.h"
#include "../Engine/vendor/BeeAlloc/include/GeneralPurposeAllocator.h"
#include "thread"


class GeneralPurposeAllocatorTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        // Инициализация ресурсов перед каждым тестовым случаем
    }

    void TearDown() override
    {
        // Освобождение ресурсов после каждого тестового случая
    }

// Общие данные для всех тестов
    void* FindBlockHeader(void* ptr)
    {
        return GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr);
    }
};

TEST_F(GeneralPurposeAllocatorTest, MemoryAllocationTest)
{
    unsigned int size = 100;
    void* ptr = GeneralPurposeAllocator::Allocate(size, 8);
    ASSERT_NE(ptr, nullptr); // Проверка, что память выделена успешно

    std::atomic<AllocatorBlockHeader>* blockHeaderPtr = static_cast<std::atomic<AllocatorBlockHeader>*>(FindBlockHeader(ptr));
    ASSERT_NE(blockHeaderPtr, nullptr); // Проверка, что блок заголовка найден

    // Проверка размера выделенной памяти
    ASSERT_GE(blockHeaderPtr->load().size, size);

    // Проверка выравнивания памяти
    ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) & (8 - 1), 0);

    // Проверка данных в BlockHeader
    ASSERT_FALSE(blockHeaderPtr->load().isFree);
    ASSERT_EQ(blockHeaderPtr->load().previousSize, 0);

    GeneralPurposeAllocator::Free(ptr);

    // Проверка данных в BlockHeader после освобождения
    ASSERT_TRUE(blockHeaderPtr->load().isFree);
    ASSERT_EQ(blockHeaderPtr->load().previousSize, 0);
    ASSERT_EQ(blockHeaderPtr->load().size, blockHeaderPtr->load().Next(blockHeaderPtr)->load().previousSize);
}

TEST_F(GeneralPurposeAllocatorTest, AllocateAndFreeTest)
{
    unsigned int size1 = 100;
    unsigned int size2 = 200;

    // Выделение первого блока памяти
    void* ptr1 = GeneralPurposeAllocator::Allocate(size1, 8);
    ASSERT_NE(ptr1, nullptr) << "Memory 1 allocation failed.";
    std::atomic<AllocatorBlockHeader>* blockHeaderPtr1 = static_cast<std::atomic<AllocatorBlockHeader>*>(FindBlockHeader(ptr1));
    ASSERT_NE(blockHeaderPtr1, nullptr) << "Memory 1 allocation failed. Invalid block header.";
    EXPECT_GE(blockHeaderPtr1->load().size, size1) << "Memory 1 has incorrect size.";

    // Выделение второго блока памяти
    void* ptr2 = GeneralPurposeAllocator::Allocate(size2, 8);
    ASSERT_NE(ptr2, nullptr) << "Memory 2 allocation failed.";
    std::atomic<AllocatorBlockHeader>* blockHeaderPtr2 = static_cast<std::atomic<AllocatorBlockHeader>*>(FindBlockHeader(ptr2));
    ASSERT_NE(blockHeaderPtr2, nullptr) << "Memory 2 allocation failed. Invalid block header.";
    EXPECT_GE(blockHeaderPtr2->load().size, size2) << "Memory 2 has incorrect size.";

    // Проверка связей между блоками
    EXPECT_EQ(blockHeaderPtr1->load().Next(blockHeaderPtr1)->load().Previous(blockHeaderPtr1->load().Next(blockHeaderPtr1)), blockHeaderPtr1) << "Memory 1 link is incorrect.";
    EXPECT_EQ(blockHeaderPtr1->load().Next(blockHeaderPtr1), blockHeaderPtr2) << "Memory 1 link to Memory 2 is incorrect.";
    EXPECT_EQ(blockHeaderPtr2->load().Previous(blockHeaderPtr2), blockHeaderPtr1) << "Memory 2 link to Memory 1 is incorrect.";

    // Освобождение первого блока памяти
    GeneralPurposeAllocator::Free(ptr1);

    // Освобождение второго блока памяти
    GeneralPurposeAllocator::Free(ptr2);

    // Проверка целостности BlockHeader после освобождения памяти
    EXPECT_NE(blockHeaderPtr1, nullptr) << "BlockHeader 1 is corrupted or memory not freed properly.";
    EXPECT_TRUE(blockHeaderPtr1->load().isFree) << "BlockHeader 1 is not marked as free.";
    EXPECT_EQ(blockHeaderPtr1->load().size, blockHeaderPtr1->load().Next(blockHeaderPtr1)->load().previousSize) << "BlockHeader 1 has incorrect size.";
}


TEST_F(GeneralPurposeAllocatorTest, MultiThreadingTest)
{
    std::thread thread1([&](){
        int size = 55;
        void* ptr = GeneralPurposeAllocator::Allocate(size, 8);
        ASSERT_NE(ptr, nullptr) << "Memory allocation failed.";
        ASSERT_NO_THROW(GeneralPurposeAllocator::Free(ptr))<< "Memory free failed on first try.";
    });
    int size = 500;
    void* ptr = GeneralPurposeAllocator::Allocate(size, 8);
    ASSERT_NE(ptr, nullptr) << "Memory allocation failed.";
    std::thread thread2([&](){
        ASSERT_NO_THROW(GeneralPurposeAllocator::Free(ptr)) << "Memory free failed on second try.";
    });
}
