//
// Created by alexl on 01.08.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    enum class MVisibility
    {
        Private,
        PrivateProtected,
        Internal,
        Protected,
        ProtectedInternal,
        Public,
    };

    enum class MType : uint32_t
    {
        None = 0x00,
        Void = 0x01,
        Boolean = 0x02,
        Char = 0x03,
        SByte = 0x04,
        Byte = 0x05,
        Int16 = 0x06,
        UInt16 = 0x07,
        Int32 = 0x08,
        UInt32 = 0x09,
        Int64 = 0x0a,
        UInt64 = 0x0b,
        Single = 0x0c,
        Double = 0x0d,
        String = 0x0e,
        Ptr = 0x0f,
        Dictionary = 0x10,
        Array = 0x11,
        List = 0x12,
        Object = 0x13,


        Vector2 = 0x20,
        Vector3 = 0x21,
        Vector4 = 0x22,
        Color = 0x23,
        Entity = 0x24,

        AssetHandle = 0x25,
        Asset = 0x26,
        Texture2D = 0x27,
        Font = 0x28,
        Prefab = 0x29,
        /*I1 = 0x04,
        U1 = 0x05,
        I2 = 0x06,
        U2 = 0x07,
        I4 = 0x08,
        U4 = 0x09,
        I8 = 0x0a,
        U8 = 0x0b,
        R4 = 0x0c,
        R8 = 0x0d,
        String = 0x0e,
        Ptr = 0x0f,
        ByRef = 0x10,
        ValueType = 0x11,
        Class = 0x12,
        Var = 0x13,
        Array = 0x14,
        GenericInst = 0x15,
        TypeByRef = 0x16,
        I = 0x18,
        U = 0x19,
        Fnptr = 0x1b,
        Object = 0x1c,
        SzArray = 0x1d,
        MVar = 0x1e,
        CmodReqd = 0x1f,
        CmodOpt = 0x20,
        Internal = 0x21,
        Modifier = 0x40,
        Sentinel = 0x41,
        Pinned = 0x45,
        Enum = 0x55,*/
    };
}