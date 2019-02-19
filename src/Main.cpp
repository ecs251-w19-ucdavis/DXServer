//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson), Min Shih                                //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include <cstdlib>
// #include <pthread.h>
#include <string>

#include "Util/Library.h"
#include "Renderer/FramebufferGL.h"

#include "DXGL.h"
#include "Communication/MainServer.h"
#include "Client.h"
    
using namespace v3d;

namespace v3d {
namespace dx { int winW = 800, winH = 800; }
void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> fbo);
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
  // pthread_t t1, t2;
  // Args *args;
  // args = (Args *)malloc(2 * sizeof(Args));
  // args[0](argv[1], 1);
  // args[1](argv[2], 2);
  // pthread_create(&t1, NULL, &RenderFunction, &args[0]);
  // pthread_create(&t2, NULL, &RenderFunction, &args[1]);
  
//  MainServer server(8080);
//  server.open();
//  while (true) {}

    return dx::DXGL_execute(argc, argv, [&]() {
        auto fbo = std::make_shared<FramebufferGL>(dx::winW, dx::winH);
        createScene(&argc, const_cast<const char **>(argv), fbo);
    });
}
