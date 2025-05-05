#pragma once

#include "Core/String.h"
#include "Locale/Locale.h"
#include "Renderer/FrameBuffer.h"
#include "RmlUi/Core/Context.h"
#include "glm/fwd.hpp"
namespace BeeEngine::RmlUi
{
    Rml::Context* CreateContext(const String& name, glm::i32vec2 sizeInPixels, Locale::Domain* domain = nullptr);
    void ResizeViewport(Rml::Context* context, glm::i32vec2 sizeInPixels);
    void UpdateAndRender(Rml::Context* context, CommandBuffer& cmd);
    WeakRef<Rml::ElementDocument*> LoadDocument(Rml::Context* context, AssetHandle handle);
    void HotReloadStyles();
    void HotReloadAll();
    void UnloadDocument(Rml::Context* context, AssetHandle handle);
    // Main context must be rendered on top and must get all events.
    void SetMainContext(Rml::Context* context);
    Rml::Context* GetMainContext();
} // namespace BeeEngine::RmlUi
