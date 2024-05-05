//
// Created by alexl on 19.08.2023.
//

#pragma once
#include "Core/Path.h"
#include <filesystem>
#include <vector>

namespace BeeEngine::Editor
{
    class AssetScanner
    {
    public:
        static std::vector<Path> GetAllAssetFiles(const Path& workingDirectory);
    };
} // namespace BeeEngine::Editor
