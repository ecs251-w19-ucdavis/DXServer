//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "SceneHandler.h"

#include "Client.h"
#include "Renderer/FramebufferGL.h"

#include <string>
#include <QImage>

namespace v3d {
    void loadModule();
    QString loadFile(const QString &fileName);
    void loadAllShaders();void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> fbo);
}

#endif