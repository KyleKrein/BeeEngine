#include "SceneImporter.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/CodeSafety/Expects.h"
#include "Scene/SceneSerializer.h"
#include <string_view>

namespace BeeEngine
{
    Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::Scene);
        BeeExpects(metadata.Location == AssetLocation::FileSystem);

        auto scene = CreateRef<Scene>();
        scene->Name = std::string_view(metadata.Name);
        scene->Location = metadata.Location;
        scene->Handle = handle;

        SceneSerializer serializer(scene);
        serializer.Deserialize(std::get<Path>(metadata.Data));
        return scene;
    }
} // namespace BeeEngine