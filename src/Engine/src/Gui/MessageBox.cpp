#include "MessageBox.h"

#if defined(WINDOWS)
#include "Platform/Windows/WindowsString.h"
#include <windows.h>

#endif
#if defined(BEE_COMPILE_SDL)
#include <SDL3/SDL.h>
#endif
namespace BeeEngine
{
    void ShowMessageBox(const String& title, const String& message, MessageBoxType type)
    {
#ifdef WINDOWS
        UINT typeFlag;
        switch (type)
        {
            case MessageBoxType::Info:
                typeFlag = MB_OK | MB_ICONINFORMATION;
                break;
            case MessageBoxType::Warning:
                typeFlag = MB_OK | MB_ICONWARNING;
                break;
            case MessageBoxType::Error:
                typeFlag = MB_OK | MB_ICONERROR;
                break;
        }
        MessageBoxW(
            NULL, Internal::WStringFromUTF8(message).c_str(), Internal::WStringFromUTF8(title).c_str(), typeFlag);
#else
        Uint32 sdlType;
        switch (type)
        {
            case MessageBoxType::Info:
                sdlType = SDL_MESSAGEBOX_INFORMATION;
                break;
            case MessageBoxType::Warning:
                sdlType = SDL_MESSAGEBOX_WARNING;
                break;
            case MessageBoxType::Error:
                sdlType = SDL_MESSAGEBOX_ERROR;
                break;
        }
        SDL_ShowSimpleMessageBox(sdlType, title.c_str(), message.c_str(), NULL);
#endif
    }
} // namespace BeeEngine