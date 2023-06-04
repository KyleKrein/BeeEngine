//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "Scene.h"
#include "Core/TypeDefines.h"
#include "yaml-cpp/emitter.h"

namespace BeeEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(Ref<Scene>& scene);
        void Serialize(std::string_view filepath);
        void SerializeBinary(std::string_view filepath);
        void Deserialize(std::string_view filepath);
        void DeserializeBinary(std::string_view filepath);
    private:
        Ref<Scene> m_Scene;

        void SerializeEntity(YAML::Emitter& emitter, Entity entity);
    };
}
