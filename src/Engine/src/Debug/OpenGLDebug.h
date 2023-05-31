//
// Created by alexl on 18.05.2023.
//

#pragma once
#include "source_location"
#define BEE_DEBUG_OPENGL 1

inline void OpenGLCheckErrors(std::string_view title, std::source_location location = std::source_location::current())
{
    int error = 1;
    while ( (error = glGetError())!= GL_NO_ERROR)
    {
        BeeCoreError("OpenGL Error: {0} in {1} {2}: {3}", error, title, std::string(location.file_name()), location.line());
    }
}

#ifdef DEBUG
    #if BEE_DEBUG_OPENGL
        #define OPENGL_CHECK_ERRORS OpenGLCheckErrors(std::source_location::current().function_name());
    #else
        #define OPENGL_CHECK_ERRORS
    #endif
#else
    #define OPENGL_CHECK_ERRORS
#endif