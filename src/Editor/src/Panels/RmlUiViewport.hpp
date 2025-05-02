#pragma once

#include "BeeEngine.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Gui/ImGui/IImGuiElement.h"
#include "Locale/Locale.h"
#include "ProjectFile.h"
#include "RmlUi/Core/Context.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneCamera.h"
#include "kdbindings/property.h"
#include <ImGuizmo.h>

namespace BeeEngine::Editor
{
    class RmlUiViewPort: public std::enable_shared_from_this<RmlUiViewPort>
    {
    public:
        RmlUiViewPort(const String& name,
                      uint32_t width,
                      uint32_t height,
                      EditorAssetManager& assetManager,
                      Locale::Domain* domain);
        void HandleEvent(EventDispatcher& event);
        void Update();
        void Render();
        bool ShouldHandleEvents() const noexcept { return m_IsHovered; }

        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        Rml::Context* GetContext() const { return m_UIContext; }

    protected:
        virtual void OnLoad(Rml::Context* context) = 0;

    private:
        uint32_t m_Width;
        uint32_t m_Height;
        glm::vec2 m_MousePosition;
        Scope<FrameBuffer> m_FrameBuffer;
        bool m_IsFocused = false;
        bool m_IsHovered = false;
        std::atomic<bool> m_FinishedLoading;
        glm::vec2 m_ViewportBounds[2]{glm::vec2(0.0f), glm::vec2(0.0f)};
        EditorAssetManager& m_AssetManager;

        Rml::Context* m_UIContext = nullptr;
        bool IsMouseInViewport();
    };
} // namespace BeeEngine::Editor
