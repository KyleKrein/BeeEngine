//
// Created by Александр Лебедев on 08.07.2023.
//

#pragma once
#include <vector>
#include <functional>
namespace Bee
{
    template<class Self, typename ...Args>
    class Event {
        friend Self;
    public:
        Event() = default;

        Event(const Event &) = delete;

        Event &operator=(const Event &) = delete;

        ~Event() = default;

        void operator+=(const std::function<void(Args...)> &listener) {
            m_Listeners.push_back(listener);
        }

        void operator-=(const std::function<void(Args...)> &listener) {
            m_Listeners.erase(std::remove(m_Listeners.begin(), m_Listeners.end(), listener), m_Listeners.end());
        }

    private:
        void Invoke(Args &&... args) {
            for (auto &listener: m_Listeners) {
                listener(std::forward<Args>(args)...);
            }
        }

        void Clear() {
            m_Listeners.clear();
        }

        std::vector<std::function<void(Args...)>> m_Listeners;
    };
}