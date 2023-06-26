//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once

#include "BufferLayout.h"

namespace BeeEngine
{
    class Vertex
    {
    public:
        void SetLayout(const BufferLayout& layout)
        {
            m_Layout = layout;
        };

        [[nodiscard]] inline const BufferLayout& GetLayout() const
        {
            return m_Layout;
        };
    private:
        BufferLayout m_Layout;
    };
}