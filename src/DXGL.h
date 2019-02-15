//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// TODO probably we should expose our thread abstraction here

#pragma once
#ifndef DXSERVER_DXGL_H
#define DXSERVER_DXGL_H

#include <cstddef>
#include <functional>

namespace v3d { namespace dx {

extern int winW, winH;
int DXGL_execute(int argc, char* argv[], const std::function<void()>& render);

}}


#endif //DXSERVER_DXGL_H
