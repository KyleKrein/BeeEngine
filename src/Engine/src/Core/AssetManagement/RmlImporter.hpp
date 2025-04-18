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
    };
} // namespace BeeEngine
