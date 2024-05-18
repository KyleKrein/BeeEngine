//
// Created by alexl on 09.06.2023.
//

#include "File.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
namespace BeeEngine
{

    std::vector<std::byte> File::ReadBinaryFile(const Path& path)
    {
        std::ifstream ifs(path.ToStdPath(), std::ios::binary | std::ios::ate);

        if (!ifs)
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }

        auto end = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        auto size = std::size_t(end - ifs.tellg());

        if (size == 0) // avoid undefined behavior
            return {};

        std::vector<std::byte> buffer(size);

        if (!ifs.read((char*)buffer.data(), buffer.size()))
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }
        ifs.close();

        return buffer;
    }

    void File::WriteBinaryFile(const Path& path, gsl::span<std::byte> content)
    {
        std::ofstream ofs(path.ToStdPath(), std::ios::out | std::ios::binary);
        if (!ofs)
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }
        ofs.write((char*)content.data(), content.size());
        ofs.close();
    }

    String File::ReadFile(const Path& path)
    {
        std::ifstream ifs(path.ToStdPath());
        if (!ifs)
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }
        std::stringstream content;
        content << ifs.rdbuf();
        ifs.close();
        return String{content.str()};
    }

    void File::WriteFile(const Path& path, const String& content)
    {
        std::ofstream ofs(path.ToStdPath(), std::ios::out);
        if (!ofs)
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }
        ofs << content;
        ofs.close();
    }

    void File::WriteFile(const Path& path, std::string_view content)
    {
        std::ofstream ofs(path.ToStdPath(), std::ios::out);
        if (!ofs)
        {
            UTF8String pathStr = path.AsUTF8() + ": " + std::strerror(errno);
            throw std::runtime_error({pathStr.begin(), pathStr.end()});
        }
        ofs << content;
        ofs.close();
    }

    size_t File::Size(const Path& path)
    {
        return std::filesystem::file_size(path.ToStdPath());
    }

    bool File::Exists(std::string_view path)
    {
        return std::filesystem::exists(path);
    }
    bool File::Exists(const std::filesystem::path& path)
    {
        return std::filesystem::exists(path);
    }

    bool File::Exists(const Path& path)
    {
        return std::filesystem::exists(path.ToStdPath());
    }

    bool File::CreateDirectory(const Path& path)
    {
        return std::filesystem::create_directory(path.ToStdPath());
    }

    bool File::CopyFile(const Path& from, const Path& to)
    {
        return std::filesystem::copy_file(from.ToStdPath(), to.ToStdPath());
    }

    bool File::IsDirectory(const Path& path)
    {
        return std::filesystem::is_directory(path.ToStdPath());
    }
} // namespace BeeEngine

#pragma clang diagnostic pop