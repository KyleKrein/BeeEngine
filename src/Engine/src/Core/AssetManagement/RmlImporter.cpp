#include "RmlImporter.hpp"
#include "Core/AssetManagement/AssetManager.h"
#include "FileSystem/File.h"
#include "Platform/RmlUi/interfaces.hpp"
#include "RmlUi/Core/Context.h"

namespace BeeEngine
{
    Ref<RmlDocument> RmlImporter::ImportRmlDocument(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::RmlDocument);
        Ref<RmlDocument> result;
        Rml::ElementDocument* document = nullptr;
        if (metadata.Location == AssetLocation::FileSystem)
        {
            document = ImportRmlDocumentFromFile(std::get<Path>(metadata.Data));
        }
        else
        {
            std::string_view rml = {reinterpret_cast<const char*>(std::get<std::span<byte>>(metadata.Data).data()),
                                    std::get<std::span<byte>>(metadata.Data).size()};
            document = ImportRmlDocumentFromString(String{rml});
        }
        if (!document)
        {
            return nullptr;
        }
        result = CreateRef<RmlDocument>(document);
        result->Name = std::string_view{metadata.Name};
        result->Handle = handle;
        result->Location = metadata.Location;
        return result;
    }

    Rml::ElementDocument* RmlImporter::ImportRmlDocumentFromFile(const Path& path)
    {
        return ImportRmlDocumentFromString(File::ReadFile(path), path);
    }
    Rml::ElementDocument* RmlImporter::ImportRmlDocumentFromString(const String& rml, std::optional<Path> filepath)
    {
        if (rml.empty())
        {
            return nullptr;
        }
        auto* context = Internal::RmlUi::GetCurrentContext();
        if (!context)
        {
            return nullptr;
        }
        return context->LoadDocumentFromMemory(
            rml.c_str(), filepath.has_value() ? filepath.value().AsCString() : "[document from asset manager]");
    }
    Ref<Rcss> RmlImporter::ImportRcss(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::RcssStyle);
        auto result = CreateRef<Rcss>();
        result->GetFileContent = [metadata]()
        {
            if (metadata.Location == AssetLocation::FileSystem)
            {
                return File::ReadFile(std::get<Path>(metadata.Data));
            }
            std::string_view rcss = {reinterpret_cast<const char*>(std::get<std::span<byte>>(metadata.Data).data()),
                                     std::get<std::span<byte>>(metadata.Data).size()};
            return String{rcss};
        };
        result->Name = std::string_view{metadata.Name};
        result->Handle = handle;
        result->Location = metadata.Location;
        return result;
    }

    Expected<void, String> ReloadRmlDocument(const RmlDocument& document)
    {
        auto* context = document.GetDocument()->GetContext();
        auto handle = document.Handle;
        auto name = String{document.Name};
        AssetManager::UnloadAsset(handle);
        Internal::RmlUi::SetCurrentContext(context);
        if (!&AssetManager::GetAsset<RmlDocument>(handle))
        {
            return Unexpected<String>(FormatString("Unable to load document {}", name));
        }
        return {};
    }
    Expected<void, String> ReloadAllRmlDocuments(const EditorAssetManager& assetManager)
    {
        for (auto& asset : assetManager.GetAssetsOfType(AssetType::RmlDocument))
        {
            auto result = ReloadRmlDocument(reinterpret_cast<const RmlDocument&>(assetManager));
            if (!result.HasValue())
            {
                return Unexpected<String>(BeeMove(result).Error());
            }
        }
        return {};
    }
} // namespace BeeEngine
