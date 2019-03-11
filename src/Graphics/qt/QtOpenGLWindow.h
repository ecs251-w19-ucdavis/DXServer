//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef V3D_GRAPHICS_QT_QTOPENGLWINDOW_H
#define V3D_GRAPHICS_QT_QTOPENGLWINDOW_H

#include "Util/GLConfig.h"

#include <QWindow>
#include <QOpenGLContext>
#include <QOffscreenSurface>

namespace v3d { namespace dx {
namespace qt {

/**
 * This is the class to create OpenGL on MacOS and Windows
 */
class OpenGLWindow: public QWindow {
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = nullptr);
    ~OpenGLWindow() final;
private:
    void makeCurrent()
    {
        _context.makeCurrent(&_offscreenSurface);
    }
    QOffscreenSurface _offscreenSurface;
    QOpenGLContext _context;
};

}}}

#endif//V3D_GRAPHICS_QT_QTOPENGLWINDOW_H
