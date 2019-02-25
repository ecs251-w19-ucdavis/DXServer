//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

// Latent usage errors can be avoided by ensuring that the .h file of a
// component parses by itself – without externally-provided declarations or
// definitions... Including the .h file as the very first line of the .c
// file ensures that no critical piece of information intrinsic to the
// physical interface of the component is missing from the .h file (or, if
// there is, that you will find out about it as soon as you try to compile
// the .c file).

// The prototype/interface header for this implementation (ie, the .h/.hh file
// that corresponds to this .cpp/.cc file).
#include "Graphics/DXGL.h"
#include "Communication/WebSocketCommunicator.h"
#include "Workspace.h"
#include "RequestHandler.h"

// Other headers from the same project, as needed.
#include "Util/Library.h"
#include "Renderer/FramebufferGL.h"

// Headers from other non-standard, non-system libraries (for example, Qt,
// Eigen, etc).
#ifndef USE_QT_OPENGL
#include <QCoreApplication>
#else
#include <QApplication>
#endif

// Headers from other "almost-standard" libraries (for example, Boost)

// Standard C++ headers (for example, iostream, functional, etc.)
#include <string>

// Standard C headers (for example, cstdint, dirent.h, etc.)
#include <cstdlib>

using namespace v3d;

namespace v3d {
namespace dx { int winW = 800, winH = 800; }
}

// class Args{
// public:
// 	Args(std::string _filename, int _id)
// 	{
// 		filename = _filename;
// 		id = _id;
// 	}
// 	std::string getFileName()
// 	{
// 		return filename;
// 	}
// 	std::string getID()
// 	{
// 		return id;
// 	}
// private:
// 	std::string filename;
// 	int id;
// }

int main(int argc, char* argv[])
{
#ifndef USE_QT_OPENGL
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication app(argc, argv);
#else
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
#endif

    dx::RequestHandler request;

    dx::WebSocketCommunicator server(8080);
    server.open();
    server.connectToRequestSlot(&request);

    dx::DXGL_create(); // load modules, load all shaders
    dx::DXGL_execute(argc, argv, [&]() {
        auto fbo = std::make_shared<FramebufferGL>(dx::winW, dx::winH);
        startWorkspace(&argc, const_cast<const char **>(argv), fbo);
    });

    return app.exec();
}
