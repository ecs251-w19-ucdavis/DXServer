//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Workspace.h"

#include "Util/Client.h"
#include "Graphics/DXGL.h"

#include <string>

#define MULTI_CLIENT_MODE 1

namespace v3d {

void startWorkspace(int *argc, const char **argv)
{
#if MULTI_CLIENT_MODE

    for (int i = 1; i < *argc; ++i) {
        dx::clients::add("-ex" + std::to_string(i));
    }
    for (int i = 1; i < *argc; ++i) {
        dx::clients::get("-ex" + std::to_string(i))->initDebug(argv[i], dx::winW, dx::winH);
    }
    for (int i = 1; i < *argc; ++i) {
        dx::clients::get("-ex" + std::to_string(i))->renderDebug();
    }


#else

    v3d::dx::SceneHandler loader(argv[1], fbo, dx::winW, dx::winH);
    loader.initData();
    loader.initScene();
    loader.updateView();
    loader.updateRenderer();

    // render
    loader.render();

    // get framebuffer image
    auto buffer = loader.copyRenderedImage();

    // save
    QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
    img.save("image.PNG", 0, -1);
    std::cout << "save file as " << "image.PNG" << std::endl;

#endif
}

}
