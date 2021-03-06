//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Graphics/DXGL.h"

#include <EGL/egl.h>
#include <glad/glad.h>

static const EGLint configAttribs[] = {
  EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
  EGL_BLUE_SIZE, 8,
  EGL_GREEN_SIZE, 8,
  EGL_RED_SIZE, 8,
  EGL_DEPTH_SIZE, 8,
  EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
  EGL_NONE
};

static const int pbufferWidth = 9;
static const int pbufferHeight = 9;

static const EGLint pbufferAttribs[] = {
  EGL_WIDTH, pbufferWidth,
  EGL_HEIGHT, pbufferHeight,
  EGL_NONE,
};

namespace v3d { namespace dx {

static EGLDisplay eglDpy;
static EGLint major, minor;
static EGLint numConfigs;
static EGLConfig eglCfg;
static EGLSurface eglSurf;
static EGLContext eglCtx;

int DXGL_init(int argc, char *argv[])
{
    // 1. Initialize EGL
    eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);
    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    // from now on use your OpenGL context
    if(!gladLoadGL()) {
        printf("Something went wrong!\n");
        exit(-1);
    }
    
    return 0;
}
    
int DXGL_exit()
{
    // 6. Terminate EGL when finished
    eglTerminate(eglDpy);

    return 0;
}

}}
