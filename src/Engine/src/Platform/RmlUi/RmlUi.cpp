#include "RmlUi.hpp"
#include "Core/AssetManagement/AssetManager.h"
#include "Gui/RmlDocument.hpp"
#include "Platform/RmlUi/interfaces.hpp"
#include "RmlUi/Core/ElementDocument.h"
#include <unordered_map>
namespace BeeEngine::RmlUi
{
    using DocumentMap = std::unordered_map<AssetHandle, Ref<Rml::ElementDocument*>>;
    static std::unordered_map<Rml::Context*, DocumentMap> g_Contexts;
    static Jobs::SpinLock g_ContextLock;
    static Rml::Context* g_MainContext = nullptr;
    Rml::Context* CreateContext(const String& name, glm::i32vec2 sizeInPixels)
    {
        auto* context = Internal::RmlUi::CreateContext(name, sizeInPixels);
        g_Contexts[context] = {};
        return context;
    }
    void ResizeViewport(Rml::Context* context, glm::i32vec2 sizeInPixels)
    {
        Internal::RmlUi::ResizeViewport(context, sizeInPixels);
    }
    void UpdateAndRender(Rml::Context* context, CommandBuffer& cmd)
    {
        std::unique_lock lock(g_ContextLock);
        Internal::RmlUi::SetCurrentContext(context);
        context->Update();
        Internal::RmlUi::BeginRendering(cmd);
        context->Render();
        Internal::RmlUi::EndRendering();
    }
    Rml::ElementDocument* LoadDocumentPtr(Rml::Context* context, AssetHandle handle)
    {
        auto* assetPtr = &AssetManager::GetAsset<RmlDocument>(handle);
        if (!assetPtr)
        {
            return {};
        }
        auto* document = context->LoadDocument(Rml::String{assetPtr->Name} + ".rml");
        if (!document)
        {
            return {};
        }
        return document;
    }
    WeakRef<Rml::ElementDocument*> LoadDocument(Rml::Context* context, AssetHandle handle)
    {
        auto* assetPtr = &AssetManager::GetAsset<RmlDocument>(handle);
        if (!assetPtr)
        {
            return {};
        }
        auto* document = context->LoadDocument(Rml::String{assetPtr->Name} + ".rml");
        if (!document)
        {
            return {};
        }
        g_Contexts.at(context)[handle] = CreateRef<Rml::ElementDocument*>(document);
        return g_Contexts.at(context).at(handle);
    }
    void UnloadDocument(Rml::Context* context, AssetHandle handle)
    {
        (*(g_Contexts.at(context).at(handle)))->Close();
        g_Contexts.at(context).erase(handle);
    }
    void HotReloadStyles()
    {
        for (auto& [context, documents] : g_Contexts)
        {
            for (auto& [handle, document] : documents)
            {
                (*document)->ReloadStyleSheet();
            }
        }
    }
    void HotReloadAll()
    {
        std::vector<std::pair<Rml::Context*, AssetHandle>> toDelete;
        for (auto& [context, documents] : g_Contexts)
        {
            for (auto& [handle, document] : documents)
            {
                (*document)->Close();
                *document = LoadDocumentPtr(context, handle);
                if (*document == nullptr)
                {
                    toDelete.emplace_back(context, handle);
                }
            }
        }
        for (auto& [context, handle] : toDelete)
        {
            g_Contexts.at(context).erase(handle);
        }
    }
    // Main context must be rendered on top and must get all events.
    void SetMainContext(Rml::Context* context)
    {
        g_MainContext = context;
    }
    Rml::Context* GetMainContext()
    {
        return g_MainContext;
    }
} // namespace BeeEngine::RmlUi
