//
// Created by alexl on 09.06.2023.
//

#pragma once

#include <string>
#include <vector>
#include <gsl/span>

namespace BeeEngine
{
    class File
    {
    public:
        static std::string ReadFile(std::string_view path);
        static std::vector<std::byte> ReadBinaryFile(std::string_view path);
        static void WriteFile(std::string_view path, std::string_view content);
        static void WriteBinaryFile(std::string_view path, gsl::span<std::byte> content);
        static size_t Size(std::string_view path);
    };
}