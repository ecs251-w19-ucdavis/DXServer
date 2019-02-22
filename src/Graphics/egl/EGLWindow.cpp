//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "DXGL.h"

#include <EGL/egl.h>
#include <glad/glad.h>

#include <thread>

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

int DXGL_execute(int argc, char *argv[], const std::function<void()> &render)
{
    std::thread main([&]()
    {
        // 1. Initialize EGL
        EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLint major, minor;
        eglInitialize(eglDpy, &major, &minor);

        // 2. Select an appropriate configuration
        EGLint numConfigs;
        EGLConfig eglCfg;
        eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

        // 3. Create a surface
        EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

        // 4. Bind the API
        eglBindAPI(EGL_OPENGL_API);

        // 5. Create a context and make it current
        EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);
        eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

        // from now on use your OpenGL context
        if(!gladLoadGL()) {
            printf("Something went wrong!\n");
            exit(-1);
        }

        // do the work
        render();

        // 6. Terminate EGL when finished
        eglTerminate(eglDpy);
    });
    main.detach(); // detach the rendering thread here because we can never join it.
    return 0;
}

}}
