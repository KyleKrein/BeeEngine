#include "RmlUiLayer.hpp"
#include "../../Engine/Assets/EmbeddedResources.h"
#include "Core/Events/EventImplementations.h"
#include "Gui/RmlDocument.hpp"
#include "Platform/Platform.h"
#include "Platform/RmlUi/RmlUi.hpp"
#include "imgui.h"
#include <RmlUi/Core.h>
RmlUiLayer::~RmlUiLayer() {}
static Rml::Context* context = nullptr;
static BeeEngine::Ref<BeeEngine::FrameBuffer> framebuffer;
struct ApplicationData
{
    bool show_text = true;
    Rml::String animal = "dog";
} my_data;
void RmlUiLayer::OnAttach()
{
    Layer::OnAttach();
    context = BeeEngine::RmlUi::CreateContext("default",
                                              {BeeEngine::WindowHandler::GetInstance()->GetWidthInPixels(),
                                               BeeEngine::WindowHandler::GetInstance()->GetHeightInPixels()});
    BeeEnsures(context != nullptr);
    BeeEngine::RmlUi::SetMainContext(context);
    BeeEngine::FrameBufferPreferences prefs;
    prefs.Width = BeeEngine::WindowHandler::GetInstance()->GetWidthInPixels();
    prefs.Height = BeeEngine::WindowHandler::GetInstance()->GetHeightInPixels();
    prefs.Attachments = {{BeeEngine::FrameBufferTextureFormat::RGBA8,
                          BeeEngine::FrameBufferTextureFormat::RedInteger,
                          BeeEngine::FrameBufferTextureFormat::Depth}};
    framebuffer = BeeEngine::FrameBuffer::Create(BeeMove(prefs));
    if (Rml::DataModelConstructor constructor = context->CreateDataModel("animals"))
    {
        constructor.Bind("show_text", &my_data.show_text);
        constructor.Bind("animal", &my_data.animal);
    }
    m_AssetManager.SetEditedAssetRegistryID({});
    m_AssetManager.GetAsset("rml.rcss");
    m_AssetManager.GetAsset("window.rcss");
    auto documentHandle = m_AssetManager.GetAsset("hello_world.rml")->Handle;
    auto documentPtr = BeeEngine::RmlUi::LoadDocument(context, documentHandle);
    if (auto document = documentPtr.lock())
    {
        (*document)->Show();
    }
}

void RmlUiLayer::OnDetach()
{
    Layer::OnDetach();
    framebuffer.reset();
}

void RmlUiLayer::OnUpdate(BeeEngine::FrameData& data)
{
    Layer::OnUpdate(data);
    auto cmd = framebuffer->Bind();
    BeeEngine::RmlUi::UpdateAndRender(context, cmd);
    framebuffer->Unbind(cmd);
    data.CopyFrameBufferImageToSwapchain(*framebuffer, 0);
}

void RmlUiLayer::OnGUIRendering()
{
    // ImGui::ShowDemoWindow();
}

void RmlUiLayer::OnEvent(BeeEngine::EventDispatcher& e)
{
    e.Dispatch<BeeEngine::WindowResizeEvent>(
        [](BeeEngine::WindowResizeEvent& event)
        {
            framebuffer->Resize(event.GetWidthInPixels(), event.GetHeightInPixels());
            return false;
        });
    Layer::OnEvent(e);
}
