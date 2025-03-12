#pragma once

#include "Core/Application.h"

#define BeeDoOnMainThread(func)                                                                                           \
    \ 
{                                                                                                                 \
        if (Application::IsMainThread())                                                                               \
        {                                                                                                              \
            func();                                                                                                    \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            Application::SubmitToMainThread(func);                                                                     \
        }                                                                                                              \
    }
