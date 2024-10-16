//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/Color4.h"
#include "MTypes.h"
#include <glm/glm.hpp>

#define BEE_RETURN_FROM_MTYPE(mtype, actualType)                                                                       \
    if constexpr (type == MType::mtype)                                                                                \
    {                                                                                                                  \
        BeeExpects(m_Type == MType::mtype);                                                                            \
        return static_cast<actualType>(*static_cast<actualType*>(m_Value));                                            \
    }

namespace BeeEngine
{
    class MClass;
    class MField;

    class MFieldValue
    {
    public:
        MFieldValue(void* value, MType type) : m_Value(value), m_Type(type) {}
        ~MFieldValue();
        template <MType type>
        auto GetValue() const
            requires(type == MType::Boolean || type == MType::SByte || type == MType::Byte || type == MType::Int16 ||
                     type == MType::UInt16 || type == MType::Int32 || type == MType::UInt32 || type == MType::Int64 ||
                     type == MType::UInt64 || type == MType::Single || type == MType::Double ||
                     type == MType::Vector2 || type == MType::Vector3 || type == MType::Vector4 ||
                     type == MType::Color || type == MType::AssetHandle || type == MType::Entity ||
                     type == MType::Asset || type == MType::Texture2D || type == MType::Font || type == MType::Prefab)
        {
            BEE_RETURN_FROM_MTYPE(Boolean, bool)
            else BEE_RETURN_FROM_MTYPE(SByte, int8_t) else BEE_RETURN_FROM_MTYPE(Byte, uint8_t) else BEE_RETURN_FROM_MTYPE(
                Int16, int16_t) else BEE_RETURN_FROM_MTYPE(UInt16,
                                                           uint16_t) else BEE_RETURN_FROM_MTYPE(Int32, int32_t) else BEE_RETURN_FROM_MTYPE(UInt32, uint32_t) else BEE_RETURN_FROM_MTYPE(Int64, int64_t) else BEE_RETURN_FROM_MTYPE(UInt64, uint64_t) else BEE_RETURN_FROM_MTYPE(Single, float) else BEE_RETURN_FROM_MTYPE(Double, double) else BEE_RETURN_FROM_MTYPE(Vector2, glm::vec2) else BEE_RETURN_FROM_MTYPE(Vector3,
                                                                                                                                                                                                                                                                                                                                                                                                                    glm::
                                                                                                                                                                                                                                                                                                                                                                                                                        vec3) else BEE_RETURN_FROM_MTYPE(Vector4,
                                                                                                                                                                                                                                                                                                                                                                                                                                                         glm::
                                                                                                                                                                                                                                                                                                                                                                                                                                                             vec4) else BEE_RETURN_FROM_MTYPE(Color,
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              Color4) else BEE_RETURN_FROM_MTYPE(AssetHandle,
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 AssetHandle) else if constexpr (type ==
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     MType::
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         Entity ||
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 type ==
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     MType::
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         Asset ||
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 type == MType::Texture2D || type == MType::Font || type == MType::Prefab)
            {
                BeeExpects(m_Type == type);
                return m_Value;
            }
            else
            {
                // Handle unsupported types or add more cases as needed
                BeeExpects(false && "Unsupported type");
            }
        }
        // don't use it directly, use GetValue instead
        void* GetValuePtr() const { return m_Value; }
        MType GetType() const { return m_Type; }
        operator void*() const { return m_Value; }

    private:
        void* m_Value = nullptr;
        MType m_Type = MType::None;
    };
    class MObject
    {
    public:
        MObject(const MClass& object);
        // MObject(MonoObject* object);
        ~MObject();
        const MClass& GetClass() const;
        void* Invoke(class MMethod& method, void** params);

        void SetFieldValue(MField& field, void* value);
        MFieldValue GetFieldValue(MField& field);
        void* GetFieldValueUnsafe(MField& field);
        String GetFieldStringValue(MField& field);

        void* GetHandle() { return m_Handle; }

    private:
        // MonoObject* m_MonoObject = nullptr;
        void* m_Handle = nullptr;
        const MClass* m_Class = nullptr;
    };
} // namespace BeeEngine
#undef BEE_RETURN_FROM_MTYPE