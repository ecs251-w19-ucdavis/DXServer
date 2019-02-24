//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_WORKSPACE_H
#define DXSERVER_WORKSPACE_H

#include "SceneHandler.h"

#include "Client.h"
#include "Renderer/FramebufferGL.h"

#include <string>
#include <QImage>

namespace v3d {
void startWorkspace(int *argc, const char **argv, std::shared_ptr<FramebufferGL> fbo);
}

#endif // DXSERVER_WORKSPACE_H