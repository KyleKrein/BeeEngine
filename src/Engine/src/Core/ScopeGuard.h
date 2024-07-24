#pragma once
#include "Move.h"
namespace BeeEngine
{
    template <typename func>
    class ScopeGuard final
    {
    public:
        ScopeGuard(func&& f) : m_Func(BeeMove(f)) {}
        ~ScopeGuard() { m_Func(); }

    private:
        func m_Func;
    };
} // namespace BeeEngine