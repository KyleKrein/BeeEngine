//
// Created by alexl on 10.11.2023.
//
#include "Utils/Commands.h"
#include "WindowsString.h"
#include <windows.h>
namespace BeeEngine
{
    void RunCommand(const String& command)
    {
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Создание командной строки
        std::wstring cmd = Internal::WStringFromUTF8("cmd.exe /C " + command);

        // Запуск командной строки
        if (!CreateProcessW(NULL,    // Имя программы
                            &cmd[0], // Командная строка
                            NULL,    // Атрибуты защиты процесса
                            NULL,    // Атрибуты защиты потока
                            FALSE,   // Ручки наследуются
                            0,       // Флаги создания
                            NULL,    // Используемые переменные среды
                            NULL,    // Текущий каталог
                            &si,     // STARTUPINFO
                            &pi      // PROCESS_INFORMATION
                            ))
        {
            // std::cerr << "Ошибка при создании процесса: " << GetLastError() << std::endl;
            return;
        }

        // Ожидание завершения команды
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Закрытие дескрипторов процесса и потока
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
} // namespace BeeEngine
