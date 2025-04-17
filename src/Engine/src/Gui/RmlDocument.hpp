#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Expected.h"
#include "Core/TypeDefines.h"
#include "RmlUi/Core/ElementDocument.h"

namespace BeeEngine
{
    class RmlDocument final : public Asset
    {
    public:
        RmlDocument(Rml::ElementDocument* document) : m_Document(document) {}
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::RmlDocument; }
        Rml::ElementDocument* GetDocument() const { return m_Document; }

        virtual ~RmlDocument() override { m_Document->Close(); }

    private:
        Rml::ElementDocument* m_Document;
    };
    class Rcss : public Asset
    {
    public:
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::RcssStyle; }
        std::function<String()> GetFileContent;
    };
    Expected<void, String> ReloadRmlDocument(const RmlDocument& document);
    Expected<void, String> ReloadAllRmlDocuments(const EditorAssetManager& assetManager);
} // namespace BeeEngine
