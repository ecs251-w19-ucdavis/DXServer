//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
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
#include "Communicator.h"
#include "TaskHandler.h"
#include "Workspace.h"
//#include "ToBeRemoved/RequestHandler.h"

// Other headers from the same project, as needed.
#include "Util/Library.h"

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

int main(int argc, char* argv[])
{
#ifndef USE_QT_OPENGL
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication app(argc, argv);
#else
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
#endif

    dx::DXGL_create(); // load modules, load all shaders

    std::shared_ptr<dx::RequestQueue> queue;
    std::shared_ptr<dx::Communicator> server;
    std::shared_ptr<dx::CPUTaskHandler> CPU_handler;
    std::shared_ptr<dx::GPUTaskHandler> GPU_handler;


    queue = std::make_shared<dx::RequestQueue>();

    server = std::make_shared<dx::Communicator>(8080);
    server->open();
    server->connectToRequestQueue(queue);

    CPU_handler = std::make_shared<dx::CPUTaskHandler>();
    CPU_handler->connectToCommunicator(server);
    CPU_handler->connectToRequestQueue(queue);

    GPU_handler = std::make_shared<dx::GPUTaskHandler>();
    GPU_handler->connectToCommunicator(server);
    GPU_handler->connectToRequestQueue(queue);

    std::thread CPU_thread([&]() {
        CPU_handler->run();

    });
    CPU_thread.detach();


    std::thread GPU_thread([&]() {
        dx::DXGL_init(argc, argv); // we should actually call this in the GPU thread
        startWorkspace(&argc, const_cast<const char **>(argv));
        GPU_handler->run();
    });
    GPU_thread.detach();


    int ret = app.exec();
    dx::DXGL_exit();
    return ret;
}
