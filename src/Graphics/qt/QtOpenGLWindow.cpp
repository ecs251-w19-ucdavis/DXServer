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

#include "DXGL.h"
#include "Util/Log.h"

namespace v3d { namespace dx {

int DXGL_execute(int argc, char* argv[], const std::function<void()>& render)
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    qt::OpenGLWindow window;
	QTimer::singleShot(0, &app, [=]() {
		render();
		QApplication::quit();
	});
    return app.exec();
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
