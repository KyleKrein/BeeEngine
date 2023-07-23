//
// Created by alexl on 02.06.2023.
//

#include "Entity.h"
#include "Components.h"

namespace BeeEngine
{

    UUID Entity::GetUUID()
    {
        return GetComponent<struct UUIDComponent>().ID;
    }
}
