//
// Created by alexl on 19.05.2023.
//
#include "BeeEngine.h"
#include "gtest/gtest.h"
#include "Core/Memory/GeneralPurposeAllocator.h"


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

    AllocatorBlockHeader* blockHeader = static_cast<AllocatorBlockHeader*>(FindBlockHeader(ptr));
    ASSERT_NE(blockHeader, nullptr); // Проверка, что блок заголовка найден

    // Проверка размера выделенной памяти
    ASSERT_GE(blockHeader->size, size);

    // Проверка выравнивания памяти
    ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) & (8 - 1), 0);

    // Проверка данных в BlockHeader
    ASSERT_FALSE(blockHeader->isFree);
    ASSERT_EQ(blockHeader->previousSize, 0);

    GeneralPurposeAllocator::Free(ptr);

    // Проверка данных в BlockHeader после освобождения
    ASSERT_TRUE(blockHeader->isFree);
    ASSERT_EQ(blockHeader->previousSize, 0);
    ASSERT_EQ(blockHeader->size, blockHeader->Next()->previousSize);
}

TEST_F(GeneralPurposeAllocatorTest, AllocateAndFreeTest)
{
    unsigned int size1 = 100;
    unsigned int size2 = 200;

    // Выделение первого блока памяти
    void* ptr1 = GeneralPurposeAllocator::Allocate(size1, 8);
    ASSERT_NE(ptr1, nullptr) << "Memory 1 allocation failed.";
    AllocatorBlockHeader* blockHeader1 = static_cast<AllocatorBlockHeader*>(FindBlockHeader(ptr1));
    ASSERT_NE(blockHeader1, nullptr) << "Memory 1 allocation failed. Invalid block header.";
    EXPECT_GE(blockHeader1->size, size1) << "Memory 1 has incorrect size.";

    // Выделение второго блока памяти
    void* ptr2 = GeneralPurposeAllocator::Allocate(size2, 8);
    ASSERT_NE(ptr2, nullptr) << "Memory 2 allocation failed.";
    AllocatorBlockHeader* blockHeader2 = static_cast<AllocatorBlockHeader*>(FindBlockHeader(ptr2));
    ASSERT_NE(blockHeader2, nullptr) << "Memory 2 allocation failed. Invalid block header.";
    EXPECT_GE(blockHeader2->size, size2) << "Memory 2 has incorrect size.";

    // Проверка связей между блоками
    EXPECT_EQ(blockHeader1->Next()->Previous(), blockHeader1) << "Memory 1 link is incorrect.";
    EXPECT_EQ(blockHeader1->Next(), blockHeader2) << "Memory 1 link to Memory 2 is incorrect.";
    EXPECT_EQ(blockHeader2->Previous(), blockHeader1) << "Memory 2 link to Memory 1 is incorrect.";

    // Освобождение первого блока памяти
    GeneralPurposeAllocator::Free(ptr1);

    // Освобождение второго блока памяти
    GeneralPurposeAllocator::Free(ptr2);

    // Проверка целостности BlockHeader после освобождения памяти
    AllocatorBlockHeader* blockHeader1AfterFree = blockHeader1;
    EXPECT_NE(blockHeader1AfterFree, nullptr) << "BlockHeader 1 is corrupted or memory not freed properly.";
    EXPECT_TRUE(blockHeader1AfterFree->isFree) << "BlockHeader 1 is not marked as free.";
    EXPECT_EQ(blockHeader1AfterFree->size, blockHeader1AfterFree->Next()->previousSize) << "BlockHeader 1 has incorrect size.";
}