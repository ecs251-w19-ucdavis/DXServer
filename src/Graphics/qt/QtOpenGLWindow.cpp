//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "QtOpenGLWindow.h"

#include <memory>

#include <QApplication>
#include <QDebug>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QTimer>
#include <QWindow>

#include "Graphics/DXGL.h"
#include "Util/Log.h"

namespace v3d { namespace dx {

// we have to make the window static so that the lifetime of this qt window
// can be identical to the lifetime of the program !
static std::shared_ptr<qt::OpenGLWindow> window;

int DXGL_init(int argc, char* argv[])
{
    window = std::make_shared<qt::OpenGLWindow>();
	return 0;
};

int DXGL_exit()
{
    return 0;
};

namespace qt {

OpenGLWindow::OpenGLWindow(QWindow *parent)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(0);
    _context.setShareContext(QOpenGLContext::globalShareContext());
    _context.setFormat(format);
    _context.create();
    _offscreenSurface.setFormat(format);
    _offscreenSurface.create();
    makeCurrent();
    if (!v3d::initGL()) {
        std::cerr << "[error] failed to initialize OpenGL extension loader" << std::endl;
        exit(EXIT_FAILURE);
    }
    log() << "[debug] OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    log() << "[debug] GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

OpenGLWindow::~OpenGLWindow()
{
    makeCurrent();
    _offscreenSurface.destroy();
}

}}}
