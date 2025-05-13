#include "RmlUi.hpp"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/Events/EventImplementations.h"
#include "Gui/RmlDocument.hpp"
#include "Platform/RmlUi/RmlUi_Platform_SDL.h"
#include "Platform/RmlUi/interfaces.hpp"
#include "RmlUi/Core/ElementDocument.h"
#include <unordered_map>
namespace BeeEngine::RmlUi
{
    using DocumentMap = std::unordered_map<AssetHandle, Ref<Rml::ElementDocument*>>;
    namespace
    {
        struct ContextData
        {
            DocumentMap Documents;
            Locale::Domain* Domain;
        };
    } // namespace
    static std::unordered_map<Rml::Context*, ContextData> g_Contexts;
    static Jobs::SpinLock g_ContextLock;
    static Rml::Context* g_MainContext = nullptr;
    Rml::Context* CreateContext(const String& name, glm::i32vec2 sizeInPixels, Locale::Domain* domain)
    {
        auto* context = Internal::RmlUi::CreateContext(name, sizeInPixels);
        g_Contexts[context] = {.Domain = domain};
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
        g_Contexts.at(context).Documents[handle] = CreateRef<Rml::ElementDocument*>(document);
        return g_Contexts.at(context).Documents.at(handle);
    }
    void UnloadDocument(Rml::Context* context, AssetHandle handle)
    {
        (*(g_Contexts.at(context).Documents.at(handle)))->Close();
        g_Contexts.at(context).Documents.erase(handle);
    }
    void HotReloadStyles()
    {
        for (auto& [context, documents] : g_Contexts)
        {
            for (auto& [handle, document] : documents.Documents)
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
            Internal::RmlUi::SetLocaleDomain(documents.Domain);
            for (auto& [handle, document] : documents.Documents)
            {
                bool isShown = (*document)->IsVisible();
                (*document)->Close();
                *document = LoadDocumentPtr(context, handle);
                if (*document == nullptr)
                {
                    toDelete.emplace_back(context, handle);
                }
                else if (isShown)
                {
                    (*document)->Show();
                }
            }
        }
        for (auto& [context, handle] : toDelete)
        {
            g_Contexts.at(context).Documents.erase(handle);
        }
    }
    // Main context must be rendered on top and must get all events.
    void SetMainContext(Rml::Context* context)
    {
        if (context)
        {
            auto* domain = g_Contexts.at(context).Domain;
            Internal::RmlUi::SetLocaleDomain(domain);
        }
        else
        {
            Internal::RmlUi::SetLocaleDomain(nullptr);
        }
        g_MainContext = context;
    }
    Rml::Context* GetMainContext()
    {
        return g_MainContext;
    }

    bool HandleEvents(Rml::Context* context, EventDispatcher& event, const std::function<float(float)>& getPhysicalSize)
    {
        BeeExpects(context);
        auto* prevContext = GetMainContext();
        SetMainContext(context);
        bool wasHandled = event.IsHandled();
        event.Dispatch<MouseButtonReleasedEvent>(
            [context](MouseButtonReleasedEvent& event)
            {
                return context->ProcessMouseButtonUp(static_cast<int>(event.GetButton()),
                                                     RmlSDL::GetKeyModifierState());
            });
        event.Dispatch<MouseButtonPressedEvent>(
            [context](MouseButtonPressedEvent& event)
            {
                return context->ProcessMouseButtonDown(static_cast<int>(event.GetButton()),
                                                       RmlSDL::GetKeyModifierState());
            });
        event.Dispatch<MouseMovedEvent>(
            [context, &getPhysicalSize](MouseMovedEvent& event)
            {
                return context->ProcessMouseMove(
                    getPhysicalSize(event.GetX()), getPhysicalSize(event.GetY()), RmlSDL::GetKeyModifierState());
            });
        event.Dispatch<MouseScrolledEvent>(
            [context](MouseScrolledEvent& event)
            {
                return context->ProcessMouseWheel(Rml::Vector2f{event.GetXOffset(), event.GetYOffset()},
                                                  RmlSDL::GetKeyModifierState());
            });
        auto convertKey = [](Key key) -> Rml::Input::KeyIdentifier
        {
            using enum Rml::Input::KeyIdentifier;
            switch (key)
            {
                case Key::A:
                    return KI_A;
                case Key::B:
                    return KI_B;
                case Key::C:
                    return KI_C;
                case Key::D:
                    return KI_D;
                case Key::E:
                    return KI_E;
                case Key::F:
                    return KI_F;
                case Key::G:
                    return KI_G;
                case Key::H:
                    return KI_H;
                case Key::I:
                    return KI_I;
                case Key::J:
                    return KI_J;
                case Key::K:
                    return KI_K;
                case Key::L:
                    return KI_L;
                case Key::M:
                    return KI_M;
                case Key::N:
                    return KI_N;
                case Key::O:
                    return KI_O;
                case Key::P:
                    return KI_P;
                case Key::Q:
                    return KI_Q;
                case Key::R:
                    return KI_R;
                case Key::S:
                    return KI_S;
                case Key::T:
                    return KI_T;
                case Key::U:
                    return KI_U;
                case Key::V:
                    return KI_V;
                case Key::W:
                    return KI_W;
                case Key::X:
                    return KI_X;
                case Key::Y:
                    return KI_Y;
                case Key::Z:
                    return KI_Z;

                case Key::D0:
                    return KI_0;
                case Key::D1:
                    return KI_1;
                case Key::D2:
                    return KI_2;
                case Key::D3:
                    return KI_3;
                case Key::D4:
                    return KI_4;
                case Key::D5:
                    return KI_5;
                case Key::D6:
                    return KI_6;
                case Key::D7:
                    return KI_7;
                case Key::D8:
                    return KI_8;
                case Key::D9:
                    return KI_9;

                case Key::Escape:
                    return KI_ESCAPE;
                case Key::Enter:
                    return KI_RETURN;
                case Key::Tab:
                    return KI_TAB;
                case Key::Backspace:
                    return KI_BACK;
                case Key::Insert:
                    return KI_INSERT;
                case Key::Delete:
                    return KI_DELETE;
                case Key::Right:
                    return KI_RIGHT;
                case Key::Left:
                    return KI_LEFT;
                case Key::Down:
                    return KI_DOWN;
                case Key::Up:
                    return KI_UP;
                case Key::PageUp:
                    return KI_PRIOR;
                case Key::PageDown:
                    return KI_NEXT;
                case Key::Home:
                    return KI_HOME;
                case Key::End:
                    return KI_END;

                case Key::CapsLock:
                    return KI_CAPITAL;
                case Key::ScrollLock:
                    return KI_SCROLL;
                case Key::NumLock:
                    return KI_NUMLOCK;
                case Key::PrintScreen:
                    return KI_SNAPSHOT;
                case Key::Pause:
                    return KI_PAUSE;

                case Key::F1:
                    return KI_F1;
                case Key::F2:
                    return KI_F2;
                case Key::F3:
                    return KI_F3;
                case Key::F4:
                    return KI_F4;
                case Key::F5:
                    return KI_F5;
                case Key::F6:
                    return KI_F6;
                case Key::F7:
                    return KI_F7;
                case Key::F8:
                    return KI_F8;
                case Key::F9:
                    return KI_F9;
                case Key::F10:
                    return KI_F10;
                case Key::F11:
                    return KI_F11;
                case Key::F12:
                    return KI_F12;

                case Key::Space:
                    return KI_SPACE;
                case Key::Apostrophe:
                    return KI_OEM_7;
                case Key::Comma:
                    return KI_OEM_COMMA;
                case Key::Minus:
                    return KI_OEM_MINUS;
                case Key::Period:
                    return KI_OEM_PERIOD;
                case Key::Slash:
                    return KI_OEM_2;
                case Key::Semicolon:
                    return KI_OEM_1;
                case Key::Equal:
                    return KI_OEM_PLUS;
                case Key::LeftBracket:
                    return KI_OEM_4;
                case Key::Backslash:
                    return KI_OEM_5;
                case Key::RightBracket:
                    return KI_OEM_6;
                case Key::GraveAccent:
                    return KI_OEM_3;

                case Key::LeftShift:
                    return KI_LSHIFT;
                case Key::LeftControl:
                    return KI_LCONTROL;
                case Key::LeftAlt:
                    return KI_LMENU;
                case Key::LeftSuper:
                    return KI_LWIN;
                case Key::RightShift:
                    return KI_RSHIFT;
                case Key::RightControl:
                    return KI_RCONTROL;
                case Key::RightAlt:
                    return KI_RMENU;
                case Key::RightSuper:
                    return KI_RWIN;

                case Key::KeyPad0:
                    return KI_NUMPAD0;
                case Key::KeyPad1:
                    return KI_NUMPAD1;
                case Key::KeyPad2:
                    return KI_NUMPAD2;
                case Key::KeyPad3:
                    return KI_NUMPAD3;
                case Key::KeyPad4:
                    return KI_NUMPAD4;
                case Key::KeyPad5:
                    return KI_NUMPAD5;
                case Key::KeyPad6:
                    return KI_NUMPAD6;
                case Key::KeyPad7:
                    return KI_NUMPAD7;
                case Key::KeyPad8:
                    return KI_NUMPAD8;
                case Key::KeyPad9:
                    return KI_NUMPAD9;
                case Key::KeyPadDecimal:
                    return KI_DECIMAL;
                case Key::KeyPadDivide:
                    return KI_DIVIDE;
                case Key::KeyPadMultiply:
                    return KI_MULTIPLY;
                case Key::KeyPadSubtract:
                    return KI_SUBTRACT;
                case Key::KeyPadAdd:
                    return KI_ADD;
                case Key::KeyPadEnter:
                    return KI_NUMPADENTER;
                case Key::KeyPadEqual:
                    return KI_OEM_NEC_EQUAL;
                case Key::Unknown:
                case Key::LastKey:
                default:
                    return KI_LAST_CUSTOM_KEY;
            }
        };
        event.Dispatch<KeyPressedEvent>(
            [context, &convertKey](KeyPressedEvent& event)
            {
                bool result = context->ProcessKeyDown(convertKey(event.GetKey()), RmlSDL::GetKeyModifierState());
                if (event.GetKey() == Key::Enter || event.GetKey() == Key::KeyPadEnter)
                {
                    result &= context->ProcessTextInput('\n');
                }
                return result;
            });
        event.Dispatch<KeyReleasedEvent>(
            [context, &convertKey](KeyReleasedEvent& event)
            { return context->ProcessKeyUp(convertKey(event.GetKey()), RmlSDL::GetKeyModifierState()); });
        event.Dispatch<CharTypedEvent>([context](CharTypedEvent& event)
                                       { return context->ProcessTextInput(Rml::String{event.AsString().c_str()}); });
        SetMainContext(prevContext);
        return !wasHandled && event.IsHandled();
    }
} // namespace BeeEngine::RmlUi
