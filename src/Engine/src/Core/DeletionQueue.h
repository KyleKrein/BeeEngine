//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once
#include <functional>
#include <deque>
#include "TypeDefines.h"

namespace BeeEngine
{
    class DeletionQueue final
    {
    public:
        static DeletionQueue& Main()
        {
            static DeletionQueue queue;
            return queue;
        }
/*
        static DeletionQueue& Frame()
        {
            static DeletionQueue queue;
            return queue;
        }
*/
        void PushFunction(std::function<void()>&& function)
        {
            m_DeletionQueue.push_back(std::move(function));
        };
        void Flush()
        {
            for (auto& function : m_DeletionQueue)
            {
                function();
            }
            m_DeletionQueue.clear();
        };

    private:
        std::deque<std::function<void()>> m_DeletionQueue;
    };
}
