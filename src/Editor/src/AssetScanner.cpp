//
// Created by alexl on 19.08.2023.
//

#include "AssetScanner.h"
#include "Core/ResourceManager.h"

namespace BeeEngine::Editor
{

    std::vector<Path> AssetScanner::GetAllAssetFiles(const Path& workingDirectory)
    {
        std::vector<Path> files;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(workingDirectory.ToStdPath()))
        {
            if (entry.path().string().contains(".beeengine"))
                continue;
            if (ResourceManager::IsAssetExtension(entry.path().extension()))
            {
                files.emplace_back(entry.path());
            }
        }
        return files;
    }
} // namespace BeeEngine::Editor
