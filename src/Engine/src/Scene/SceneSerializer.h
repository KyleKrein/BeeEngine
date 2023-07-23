//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "Scene.h"
#include "Core/TypeDefines.h"
#include "yaml-cpp/emitter.h"
#include <filesystem>
namespace BeeEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(Ref<Scene>& scene);
        void Serialize(const std::filesystem::path& filepath);
        void SerializeBinary(const std::filesystem::path& filepath);
        void Deserialize(const std::filesystem::path& filepath);
        void DeserializeBinary(const std::filesystem::path& filepath);
    private:
        Ref<Scene> m_Scene;

        void SerializeEntity(YAML::Emitter& emitter, Entity entity);
    };
}
