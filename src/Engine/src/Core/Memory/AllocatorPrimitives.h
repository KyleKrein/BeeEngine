//
// Created by alexl on 17.11.2023.
//

#pragma once
#include <concepts>

namespace BeeEngine
{
    struct MemoryBlock
    {
        MemoryBlock() = default;
        MemoryBlock(void* ptr, size_t size) noexcept : Ptr(ptr), Size(size) {}
        void* Ptr = nullptr;
        size_t Size = 0;

        bool operator==(const MemoryBlock& other) const { return Ptr == other.Ptr && Size == other.Size; }
        bool operator!=(const MemoryBlock& other) const { return !(*this == other); }
    };

    template <typename T>
    class Ptr
    {
    public:
        Ptr() noexcept { m_Block = MemoryBlock{nullptr, 0}; }
        Ptr(MemoryBlock block) noexcept { m_Block = block; }
        T& operator*() const noexcept { return *static_cast<T>(m_Block.Ptr); }
        T* operator->() const noexcept { return static_cast<T*>(m_Block.Ptr); }
        T* Get() const noexcept { return static_cast<T*>(m_Block.Ptr); }
        operator bool() const noexcept { return m_Block.Ptr != nullptr; }
        MemoryBlock GetMemoryBlock() const noexcept { return m_Block; }

    private:
        MemoryBlock m_Block;
    };

    template <typename T>
    concept MemoryAllocator = requires(T a) {
        { a.Allocate(0) } -> std::same_as<MemoryBlock>;
        { a.Deallocate(MemoryBlock{}) } -> std::same_as<void>;
        { a.AllocateAligned(0, 8) } -> std::same_as<MemoryBlock>;
        { a.DeallocateAligned(MemoryBlock{}, 0) } -> std::same_as<void>;
        { a.Owns(MemoryBlock{}) } -> std::same_as<bool>;
        { a.DeallocateAll() } -> std::same_as<void>;
    };

    inline void* AlignMemoryAddress(void* ptr, size_t alignment)
    {
        return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1));
    }
    inline uintptr_t AlignMemoryAddress(uintptr_t ptr, size_t alignment)
    {
        return (ptr + alignment - 1) & ~(alignment - 1);
    }
} // namespace BeeEngine