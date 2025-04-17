#pragma once
#include "AssetMetadata.h"
#include "Gui/RmlDocument.hpp"
#include <optional>

namespace BeeEngine
{
    class RmlImporter
    {
    public:
        static Ref<RmlDocument> ImportRmlDocument(AssetHandle handle, const AssetMetadata& metadata);
        static Ref<Rcss> ImportRcss(AssetHandle handle, const AssetMetadata& metadata);
        static Rml::ElementDocument* ImportRmlDocumentFromFile(const Path& path);
        static Rml::ElementDocument* ImportRmlDocumentFromString(const String& rml,
                                                                 std::optional<Path> filepath = std::nullopt);
    };
} // namespace BeeEngine
