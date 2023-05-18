//
// Created by alexl on 18.05.2023.
//

#pragma once

#define BEE_DEBUG_OPENGL 1

#ifdef DEBUG
    #if BEE_DEBUG_OPENGL
        #define OPENGL_CHECK_ERRORS { \
        auto error = glGetError();                      \
        while ( error != GL_NO_ERROR)\
        {\
            BeeCoreError("OpenGL Error: {0}, {1}: {2}", error, std::string(__FILE__), __LINE__);\
            error = glGetError();\
        }\
        }
    #else
        #define OPENGL_CHECK_ERRORS
    #endif
#else
    #define OPENGL_CHECK_ERRORS
#endif