//
// Created by Александр Лебедев on 23.07.2023.
//

#pragma once
#include <string>
namespace BeeEngine
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
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