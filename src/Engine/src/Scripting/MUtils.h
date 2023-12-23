//
// Created by alexl on 01.08.2023.
//

#pragma once
#include "MTypes.h"
namespace BeeEngine
{
    class MUtils
    {
    public:
        static MType ManagedNameToMType(const String& name);
        static MType StringToMType(const String& name);
        static const char* MTypeToString(MType type);

        static MVisibility MonoFieldFlagsToVisibility(uint32_t flags);

        static bool IsSutableForEdit(const class MField& field);

        static MVisibility MonoMethodFlagsToVisibility(uint32_t flags);

        static size_t SizeOfMType(MType type);
        static void RegisterThread();
    };
}
