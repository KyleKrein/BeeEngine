//
// Created by Александр Лебедев on 23.07.2023.
//

#include "UUID.h"
#include <random>

namespace BeeEngine
{
    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_RandomEngine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
    UUID::UUID() : m_UUID(s_UniformDistribution(s_RandomEngine)) {}

    REFLECT_STRUCT_BEGIN(UUID)
    REFLECT_STRUCT_MEMBER(m_UUID)
    REFLECT_STRUCT_END()
} // namespace BeeEngine
