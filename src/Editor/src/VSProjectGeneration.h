//
// Created by alexl on 03.08.2023.
//

#pragma once
#include "Core/Path.h"
#include <filesystem>

namespace BeeEngine
{
    class VSProjectGeneration
    {
    public:
        static std::vector<Path> GetSourceFiles(const Path& path);
        static void GenerateProject(const Path& path, const std::vector<Path>& sources, const std::string& projectName);

    private:
    };
} // namespace BeeEngine