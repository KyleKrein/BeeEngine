//
// Created by alexl on 09.06.2023.
//

#pragma once

namespace BeeEngine
{
    class GraphicsQueue
    {
    public:
        GraphicsQueue() = default;
        virtual ~GraphicsQueue() = default;

        GraphicsQueue(const GraphicsQueue&) = delete;
        GraphicsQueue& operator=(const GraphicsQueue&) = delete;

        //virtual void Submit() = 0;
        //virtual void Present() = 0;
    };
}
