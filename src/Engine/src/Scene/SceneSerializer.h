//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "Core/Path.h"
#include "Core/TypeDefines.h"
#include "Scene.h"
#include "yaml-cpp/emitter.h"
namespace BeeEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(Ref<Scene>& scene);
        void Serialize(const Path& filepath);
        String SerializeToString();
        String SerializeEntityToString(Entity entity);
        void SerializeBinary(const Path& filepath);
        void Deserialize(const Path& filepath);
        void DeserializeFromString(const String& string);
        Entity DeserializeEntityFromString(const String& string);
        void DeserializeBinary(const Path& filepath);

    private:
        Ref<Scene> m_Scene;

        void SerializeEntity(YAML::Emitter& emitter, Entity entity);

        Entity DeserializeEntity(YAML::Node& entities);
    };
} // namespace BeeEngine
