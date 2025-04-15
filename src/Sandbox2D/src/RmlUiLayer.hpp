#pragma once
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/Layer.h"
#include "Renderer/Renderer.h"

class RmlUiLayer : public BeeEngine::Layer
{
public:
    ~RmlUiLayer() override;

    void OnAttach() override;

    void OnDetach() override;

    void OnUpdate(BeeEngine::FrameData& data) override;

    void OnGUIRendering() override;

    void OnEvent(BeeEngine::EventDispatcher& e) override;

private:
    BeeEngine::EditorAssetManager m_AssetManager;
};
