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

#include "DXGL.h"
#include "Client.h"

#include "Util/Library.h"

#include "Util/SourceCodeManager.h"
#include "Renderer/FramebufferGL.h"
//#include "Engine/RegularGridPipelineGL.h"
//#include "Engine/TetraGridPipelineGL.h"

#include <string>
#include <QImage>
#include <QFile>

namespace v3d {
    void loadModule();
    QString loadFile(const QString &fileName);
    void loadAllShaders();void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> fbo);
}

#endif