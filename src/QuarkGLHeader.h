//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef V3D_QUARKGLHEADER_H
#define V3D_QUARKGLHEADER_H

#include <cstddef>
#include <functional>

namespace v3d { namespace quark {

extern int winW, winH;
int QuarkExecute(int argc, char* argv[], const std::function<void()>& render);

}}


#endif //V3D_QUARKGLHEADER_H
