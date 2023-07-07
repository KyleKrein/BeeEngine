//
// Created by alexl on 09.06.2023.
//

#pragma once

namespace BeeEngine
{
    class Surface
    {
    public:
        Surface() = default;
        virtual ~Surface() = default;

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;

        //virtual void Submit() = 0;
        //virtual void Present() = 0;

    };
}
