//
// Created by alexl on 19.08.2023.
//

#pragma once
#include <filesystem>
#include <vector>

namespace BeeEngine::Editor
{
    class AssetScanner
    {
        static std::vector<std::filesystem::path> GetAllAssetFiles(const std::filesystem::path& workingDirectory);
    };
}
