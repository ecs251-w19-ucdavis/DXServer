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

#include "QuarkGLHeader.h"
#include "Communication/MainServer.h"
    
using namespace v3d;

void testModule(int* argc, const char** argv)
{
    auto &repo = *v3d::LibraryRepository::GetInstance();
    repo.addDefaultLibrary();
    if (!repo.libraryExists("vidi3d_core")) {
        repo.add("vidi3d_core");
    }
    void *TestEcho = repo.getSymbol("V3DEcho");
    if (!TestEcho) {
        throw std::runtime_error("Could not find function");
    }
    auto *TestEchoFcn = (void (*)(const std::string &str)) TestEcho;
    TestEchoFcn("hello world");

}

namespace v3d {
namespace quark { int winW = 800, winH = 800; }
void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> &&fbo);
}

int main(int argc, char* argv[])
{
    testModule(&argc, const_cast<const char**>(argv));
    return quark::QuarkExecute(argc, argv, [&]()
    {
        auto fbo = std::make_shared<FramebufferGL>(quark::winW, quark::winH);
        createScene(&argc, const_cast<const char **>(argv), std::move(fbo));
    });
}
