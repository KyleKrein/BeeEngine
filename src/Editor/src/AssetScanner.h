//
// Created by alexl on 19.08.2023.
//

#pragma once
#include <filesystem>
#include <vector>
#include "Core/Path.h"

namespace BeeEngine::Editor
{
    class AssetScanner
    {
        public:
        static std::vector<Path> GetAllAssetFiles(const Path& workingDirectory);
    };
}
