//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef V3D_QUARKGLHEADER_INTERNAL_H
#define V3D_QUARKGLHEADER_INTERNAL_H

// ======================================================================== //
// Copyright SCI Institute, University of Utah, 2018
// ======================================================================== //
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
# undef APIENTRY
# define GLFW_EXPOSE_NATIVE_WIN32
# define GLFW_EXPOSE_NATIVE_WGL
# include <GLFW/glfw3native.h>
#endif
#include <iostream>
//! @name error check helper from EPFL ICG class
static inline const char *ErrorString(GLenum error) {
    const char *msg;
    switch (error) {
#define Case(Token)  case Token: msg = #Token; break;
        Case(GL_INVALID_ENUM);
        Case(GL_INVALID_VALUE);
        Case(GL_INVALID_OPERATION);
        Case(GL_INVALID_FRAMEBUFFER_OPERATION);
        Case(GL_NO_ERROR);
        Case(GL_OUT_OF_MEMORY);
#undef Case
    }
    return msg;
}

//! @name check error
static inline void _glCheckError
    (const char *file, int line, const char *comment) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n",
                comment, file, line, ErrorString(error));
    }
}
#ifndef NDEBUG
# define check_error_gl(x) _glCheckError(__FILE__, __LINE__, x)
#else
# define check_error_gl(x) ((void)0)
#endif

#endif //V3D_QUARKGLHEADER_INTERNAL_H
