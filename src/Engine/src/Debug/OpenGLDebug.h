//
// Created by alexl on 18.05.2023.
//

#pragma once
#include "version"

#define BEE_DEBUG_OPENGL 1

#ifdef __cpp_lib_source_location
#include "source_location"
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

#else
inline void OpenGLCheckErrors(std::string_view title, std::string_view file, std::string_view line)
{
    int error = 1;
    while ( (error = glGetError())!= GL_NO_ERROR)
    {
        BeeCoreError("OpenGL Error: {0} in {1} {2}: {3}", error, title, file, line);
    }
}

#ifdef DEBUG
#if BEE_DEBUG_OPENGL
#define OPENGL_CHECK_ERRORS OpenGLCheckErrors(__FUNCTION__, __FILE__, std::to_string(__LINE__));
#else
#define OPENGL_CHECK_ERRORS
#endif
#else
#define OPENGL_CHECK_ERRORS
#endif
#endif

