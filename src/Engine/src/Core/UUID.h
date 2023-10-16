//
// Created by Александр Лебедев on 23.07.2023.
//

#pragma once
#include <string>
#include "Core/Reflection.h"
namespace BeeEngine
{
    class UUID
    {
        REFLECT()
    public:
        UUID();
        constexpr UUID(uint64_t uuid)
            : m_UUID(uuid)
        {}
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };
}

namespace std
{
        template<>
        struct hash<BeeEngine::UUID>
        {
            std::size_t operator()(const BeeEngine::UUID& uuid) const
            {
                return hash<uint64_t>()((uint64_t)uuid);
            }
        };

}