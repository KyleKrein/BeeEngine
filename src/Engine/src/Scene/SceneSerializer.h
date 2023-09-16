//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "Scene.h"
#include "Core/TypeDefines.h"
#include "yaml-cpp/emitter.h"
#include "Core/Path.h"
namespace BeeEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(Ref<Scene>& scene);
        void Serialize(const Path& filepath);
        void SerializeBinary(const Path& filepath);
        void Deserialize(const Path& filepath);
        void DeserializeBinary(const Path& filepath);
    private:
        Ref<Scene> m_Scene;

        void SerializeEntity(YAML::Emitter& emitter, Entity entity);
    };
}
