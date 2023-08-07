//
// Created by alexl on 03.08.2023.
//

#pragma once
#include <filesystem>
namespace BeeEngine
{
    class VSProjectGeneration
    {
    public:
        static void GenerateAssemblyInfoFile(const std::filesystem::path& path, std::string_view projectName);
        static std::vector<std::filesystem::path> GetSourceFiles(const std::filesystem::path& path);
        static void GenerateProject(const std::filesystem::path& path, const std::vector<std::filesystem::path>& sources, const std::string& projectName);
    private:
    };
}