//
// Created by alexl on 03.08.2023.
//

#pragma once
#include <filesystem>
#include "Core/Path.h"

namespace BeeEngine
{
    class VSProjectGeneration
    {
    public:
        static std::vector<Path> GetSourceFiles(const Path& path);
        static void GenerateProject(const Path& path, const std::vector<Path>& sources, const std::string& projectName);
    private:
    };
}