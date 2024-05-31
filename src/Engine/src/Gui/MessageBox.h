#pragma once
#include "Core/String.h"
namespace BeeEngine
{
    enum class MessageBoxType
    {
        Info,
        Warning,
        Error
    };
    void ShowMessageBox(const String& title, const String& message, MessageBoxType type = MessageBoxType::Info);
} // namespace BeeEngine