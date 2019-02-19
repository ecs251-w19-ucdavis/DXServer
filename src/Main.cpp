//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include <cstdlib>

#include "Util/Library.h"
#include "Renderer/FramebufferGL.h"

#include "DXGL.h"
#include "Communication/MainServer.h"
    
using namespace v3d;

namespace v3d {
namespace dx { int winW = 800, winH = 800; }
void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> &&fbo);
}

int main(int argc, char* argv[])
{
    return dx::DXGL_execute(argc, argv, [&]() {
        auto fbo = std::make_shared<FramebufferGL>(dx::winW, dx::winH);
        createScene(&argc, const_cast<const char **>(argv), std::move(fbo));
    });
}
