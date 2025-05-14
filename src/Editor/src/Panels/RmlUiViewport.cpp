#include "RmlUiViewport.hpp"
#include "AssetPanel.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Events/Event.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "Debug/Instrumentor.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Platform/RmlUi/RmlUi.hpp"
#include "Platform/RmlUi/RmlUi_Platform_SDL.h"
#include "Renderer/SceneRenderer.h"
#include "RmlUi/Core/Context.h"
#include "RmlUi/Core/DataModelHandle.h"
#include "RmlUi/Core/ElementDocument.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"
#include "Scripting/ScriptingEngine.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "imgui.h"
#include <array>
#include <glm/gtc/type_ptr.hpp>

namespace BeeEngine::Editor
{
    template <typename T, typename R = int32_t>
    static R PhysicalSize(T size)
    {
        return static_cast<int32_t>(size) *
               static_cast<int32_t>(WindowHandler::GetInstance()->GetScaleFactor() +
                                    1.6); //+1.6 as dirty fix, because everything looks like shit in imgui :/
    }
    RmlUiViewPort::RmlUiViewPort(
        const String& name, uint32_t width, uint32_t height, EditorAssetManager& assetManager, Locale::Domain* domain)
        : m_Width(width), m_Height(height), m_FrameBuffer(nullptr), m_AssetManager(assetManager)
    {
        FrameBufferPreferences preferences;
        preferences.Width = PhysicalSize(m_Width);
        preferences.Height = PhysicalSize(m_Height);
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        m_FrameBuffer = FrameBuffer::Create(preferences);
        m_UIContext = RmlUi::CreateContext(name.c_str(), {width, height}, domain);
        Jobs::Schedule(Jobs::CreateJob(
            [weakSelf = this->weak_from_this()]()
            {
                Jobs::this_job::SleepFor(Time::millisecondsD{10});
                if (auto self = weakSelf.lock())
                {
                    self->OnLoad(self->m_UIContext);
                    self->m_FinishedLoading = true;
                }
            })); // To avoid calling pure virtual function on constructor
    }

    static glm::mat4 MakeUIMatrix(float width, float height)
    {
        return glm::ortho(0.0f, width, height, 0.0f, -10000.f, 10000.f);
    }

    void RmlUiViewPort::HandleEvent(EventDispatcher& event)
    {
        if (!ShouldHandleEvents())
            return;
        if (event.GetCategory() & EventCategory::App)
        {
            return;
        }
        if (event.GetType() != EventType::MouseMoved && IsMouseInViewport())
        {
            RmlUi::HandleEvents(m_UIContext, event, PhysicalSize<float, float>);
        }
    }

    void RmlUiViewPort::Update()
    {
        BEE_PROFILE_FUNCTION();
        auto cmd = m_FrameBuffer->Bind();

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

        int mouseX = static_cast<int>(mx);
        int mouseY = static_cast<int>(my);

        m_MousePosition = {mx, my};
        RmlUi::UpdateAndRender(m_UIContext, cmd);
        m_FrameBuffer->Unbind(cmd);
        if (IsMouseInViewport())
        {
            MouseMovedEvent mouseMovedEvent(m_MousePosition.x, m_MousePosition.y);
            EventDispatcher dispatcher{&mouseMovedEvent};
            RmlUi::HandleEvents(m_UIContext, dispatcher, PhysicalSize<float, float>);
        }
    }

    void RmlUiViewPort::Render()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("##Viewport",
                     nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
        m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();

        auto& io = ImGui::GetIO();
        auto logicalSize = ImGui::GetContentRegionAvail();

        if (narrow_cast<float>(m_Width) != logicalSize.x || narrow_cast<float>(m_Height) != logicalSize.y)
        {
            m_Width = narrow_cast<uint32_t>(logicalSize.x);
            m_Height = narrow_cast<uint32_t>(logicalSize.y);
            auto scaledWidth = PhysicalSize(logicalSize.x);
            auto scaledHeight = PhysicalSize(logicalSize.y);
            m_FrameBuffer->Resize(scaledWidth, scaledHeight);
            auto uimat4 = MakeUIMatrix(m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight());
            Rml::Vector2i dimensions(scaledWidth, scaledHeight);
            m_UIContext->SetDimensions(dimensions);
            m_UIContext->SetDensityIndependentPixelRatio(PhysicalSize(logicalSize.x) /
                                                         static_cast<int32_t>(logicalSize.x));
            RmlUi::ResizeViewport(m_UIContext, {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()});
        }
        auto textureID = m_FrameBuffer->GetColorAttachmentImGuiRendererID(0);
        BeeExpects(textureID != 0);
        ImGui::Image((ImTextureID)textureID, {static_cast<float>(m_Width), static_cast<float>(m_Height)});

        ImGui::End();
        ImGui::PopStyleVar();
    }

    bool RmlUiViewPort::IsMouseInViewport()
    {
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        return m_MousePosition.x >= 0 && m_MousePosition.y >= 0 && m_MousePosition.x < viewportSize.x &&
               m_MousePosition.y < viewportSize.y;
    }
} // namespace BeeEngine::Editor
