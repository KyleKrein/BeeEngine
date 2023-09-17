//
// Created by alexl on 09.06.2023.
//

#pragma once

#include <string>
#include <vector>
#include <gsl/span>
#include <filesystem>
#include "Core/Path.h"
#undef CreateDirectory
#undef CopyFile
namespace BeeEngine
{
    class File
    {
    public:
        static std::string ReadFile(const Path& path);
        static std::vector<std::byte> ReadBinaryFile(const Path& path);
        static void WriteFile(const Path& path, std::string_view content);
        static void WriteBinaryFile(const Path& path, gsl::span<std::byte> content);
        static size_t Size(const Path& path);
        static bool Exists(std::string_view path);
        static bool Exists(const std::filesystem::path& path);
        static bool Exists(const Path& path);
        static bool CreateDirectory(const Path& path);
        static bool CopyFile(const Path& from, const Path& to);
        static bool IsDirectory(const Path& path);
    };
}
