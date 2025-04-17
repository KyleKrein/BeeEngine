#include "RmlUiLayer.hpp"
#include "../../Engine/Assets/EmbeddedResources.h"
#include "Core/Events/EventImplementations.h"
#include "Gui/RmlDocument.hpp"
#include "Platform/Platform.h"
#include "Platform/RmlUi/interfaces.hpp"
#include "imgui.h"
#include <RmlUi/Core.h>
RmlUiLayer::~RmlUiLayer() {}
static Rml::Context* context = nullptr;
struct ApplicationData
{
    bool show_text = true;
    Rml::String animal = "dog";
} my_data;
void RmlUiLayer::OnAttach()
{
    Layer::OnAttach();
    context = BeeEngine::Internal::RmlUi::CreateContext("default",
                                                        {BeeEngine::WindowHandler::GetInstance()->GetWidthInPixels(),
                                                         BeeEngine::WindowHandler::GetInstance()->GetHeightInPixels()});
    BeeEnsures(context != nullptr);
    BeeEngine::Internal::RmlUi::SetCurrentContext(context);
    if (Rml::DataModelConstructor constructor = context->CreateDataModel("animals"))
    {
        constructor.Bind("show_text", &my_data.show_text);
        constructor.Bind("animal", &my_data.animal);
    }
    m_AssetManager.SetEditedAssetRegistryID({});
    m_AssetManager.GetAsset("rml.rcss");
    m_AssetManager.GetAsset("window.rcss");
    auto* document = BeeEngine::AssetManager::GetAsset<BeeEngine::RmlDocument>("hello_world.rml").GetDocument();
    document->Show();
}

void RmlUiLayer::OnDetach()
{
    Layer::OnDetach();
}

void RmlUiLayer::OnUpdate(BeeEngine::FrameData& data)
{
    Layer::OnUpdate(data);
    if (auto* rmluiContext = Rml::GetContext("default"))
    {
        rmluiContext->Update();
        BeeEngine::Internal::RmlUi::BeginRendering();
        rmluiContext->Render();
        BeeEngine::Internal::RmlUi::EndRendering();
        data.CopyFrameBufferImageToSwapchain(BeeEngine::Internal::RmlUi::GetFrameBuffer(rmluiContext), 0);
    }
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
            // context->SetDimensions({event.GetWidthInPixels(), event.GetHeightInPixels()});
            BeeEngine::Internal::RmlUi::ResizeFramebuffer(context,
                                                          {event.GetWidthInPixels(), event.GetHeightInPixels()});
            return false;
        });
    Layer::OnEvent(e);
}
