#pragma once
#include "Move.h"
namespace BeeEngine
{
    template <typename func>
    class ScopeGuard final
    {
    public:
        void Cancel() { m_Cancelled = true; }
        ScopeGuard(func&& f) : m_Func(BeeMove(f)) {}
        ~ScopeGuard()
        {
            if (!m_Cancelled)
            {
                m_Func();
            }
        }

    private:
        func m_Func;
        bool m_Cancelled = false;
    };
} // namespace BeeEngine