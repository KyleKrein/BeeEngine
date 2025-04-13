//
// Created by alexl on 09.06.2023.
//

#include "File.h"
#include "Core/Logging/Log.h"
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

    void File::WriteBinaryFile(const Path& path, std::span<std::byte> content)
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
            BeeCoreError("Failed to write file: {}", pathStr);
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
            BeeCoreError("Failed to write file: {}", pathStr);
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

    void File::Copy(const Path& from, const Path& to)
    {
        // FIXME use check for permissions instead
        std::error_code err;
        // std::filesystem::copy(from.ToStdPath(),
        //                       to.ToStdPath(),
        // std::filesystem::copy_options::recursive |
        // std::filesystem::copy_options::overwrite_existing,
        //                              err);
        // if (!err)
        //{
        //    return;
        //}

        if (IsDirectory(from))
        {
            EnsureDirectory(to);
            for (const auto& p : std::filesystem::recursive_directory_iterator(from.ToStdPath()))
            {
                Copy(p.path(), to / p.path().filename());
            }
            return;
        }
        Copy(from, to);
    }

    bool File::CopyFile(const Path& from, const Path& to)
    {
        // FIXME Use check for permissions instead
        std::error_code err;
        // bool result = std::filesystem::copy_file(
        //     from.ToStdPath(), to.ToStdPath(), std::filesystem::copy_options::overwrite_existing, err);
        // if (!result)
        {
            // BeeCoreWarn("Error copying {}: {}. Copy to and from memory will be used instead", from, err.message());
            auto data = ReadBinaryFile(from);
            if (Exists(to))
            {
                std::filesystem::remove_all(to.ToStdPath(), err);
            }
            WriteBinaryFile(to, data);
        }
        return true;
    }

    bool File::IsDirectory(const Path& path)
    {
        return std::filesystem::is_directory(path.ToStdPath());
    }
    bool File::EnsureDirectory(const Path& path)
    {
        try
        {
            BeeCoreTrace("Ensuring {} directory", path);
            if (!path.IsEmpty() && !File::Exists(path))
            {
                EnsureDirectory(path.GetParent());
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            BeeCoreError("Unable to ensure, that directory {} exists: {}", path, error);
            return false;
        }
        try
        {
            if (!File::Exists(path))
            {
                File::CreateDirectory(path);
            }
        }
        catch (const std::filesystem::filesystem_error& error)
        {
            BeeCoreError("Unable to ensure, that directory {} exists: {}", path, error);
            return false;
        }
        return true;
    }
} // namespace BeeEngine

#pragma clang diagnostic pop
