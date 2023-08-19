//
// Created by alexl on 19.08.2023.
//

#include "AssetScanner.h"
#include "Core/ResourceManager.h"


namespace BeeEngine::Editor
{

    std::vector<std::filesystem::path> AssetScanner::GetAllAssetFiles(const std::filesystem::path &workingDirectory)
    {
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(workingDirectory))
        {
            if(entry.path().string().contains(".beeengine"))
                continue;
            if (ResourceManager::IsAssetExtension(entry.path().extension()))
            {
                files.push_back(entry.path());
            }
        }
        return files;
    }
}
