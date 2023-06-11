//
// Created by alexl on 09.06.2023.
//

#include <fstream>
#include <cstring>
#include <filesystem>
#include "File.h"


namespace BeeEngine
{

    std::vector<std::byte> File::ReadBinaryFile(std::string_view path)
    {

        std::ifstream ifs(std::string(path), std::ios::binary|std::ios::ate);

        if(!ifs)
            throw std::runtime_error(std::string(path) + ": " + std::strerror(errno));

        auto end = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        auto size = std::size_t(end - ifs.tellg());

        if(size == 0) // avoid undefined behavior
            return {};

        std::vector<std::byte> buffer(size);

        if(!ifs.read((char*)buffer.data(), buffer.size()))
            throw std::runtime_error(std::string(path) + ": " + std::strerror(errno));
        ifs.close();

        return buffer;
    }

    void File::WriteBinaryFile(std::string_view path, gsl::span<std::byte> content)
    {
        std::ofstream ofs(std::string(path), std::ios::out |std::ios::binary);
        if(!ofs)
            throw std::runtime_error(std::string(path) + ": " + std::strerror(errno));
        ofs.write((char*)content.data(), content.size());
        ofs.close();
    }

    std::string File::ReadFile(std::string_view path)
    {
        std::ifstream ifs((std::string(path)));
        if(!ifs)
            throw std::runtime_error(std::string(path) + ": " + std::strerror(errno));
        std::string content;
        ifs>>content;
        ifs.close();
        return std::move(content);
    }

    void File::WriteFile(std::string_view path, std::string_view content)
    {
        std::ofstream ofs(std::string(path), std::ios::out);
        if(!ofs)
            throw std::runtime_error(std::string(path) + ": " + std::strerror(errno));
        ofs << content;
        ofs.close();
    }

    size_t File::Size(std::string_view path)
    {
        return std::filesystem::file_size(path);
    }

    bool File::Exists(std::string_view path)
    {
        return std::filesystem::exists(path);
    }
}
