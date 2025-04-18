#include "RmlImporter.hpp"
#include "Core/AssetManagement/AssetManager.h"
#include "FileSystem/File.h"
#include "Platform/RmlUi/interfaces.hpp"
#include "RmlUi/Core/Context.h"

namespace BeeEngine
{
    namespace
    {
        template <typename T>
        Ref<T> ImportDocument(AssetHandle handle, const AssetMetadata& metadata)
        {
            auto result = CreateRef<T>();
            result->GetFileContent = [metadata]()
            {
                if (metadata.Location == AssetLocation::FileSystem)
                {
                    return File::ReadFile(std::get<Path>(metadata.Data));
                }
                std::string_view data = {reinterpret_cast<const char*>(std::get<std::span<byte>>(metadata.Data).data()),
                                         std::get<std::span<byte>>(metadata.Data).size()};
                return String{data};
            };
            result->Name = std::string_view{metadata.Name};
            result->Handle = handle;
            result->Location = metadata.Location;
            return result;
        }
    } // namespace

    Ref<RmlDocument> RmlImporter::ImportRmlDocument(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::RmlDocument);
        return ImportDocument<RmlDocument>(handle, metadata);
    }
    Ref<Rcss> RmlImporter::ImportRcss(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::RcssStyle);
        return ImportDocument<Rcss>(handle, metadata);
    }
} // namespace BeeEngine
