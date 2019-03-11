//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Experiment.h"

#include "Util/Client.h"
#include "Graphics/DXGL.h"

#include <string>

#define MULTI_CLIENT_MODE 1

namespace v3d {

void startExperiment(int *argc, const char **argv)
{
#if MULTI_CLIENT_MODE

    for (int i = 1; i < *argc; ++i) {
        dx::clients::add("-ex" + std::to_string(i));
    }
    for (int i = 1; i < *argc; ++i) {
        auto c = dx::clients::get("-ex" + std::to_string(i));
        c->openProject(argv[i], dx::winW, dx::winH);
        c->initGL();
    }
    for (int i = 1; i < *argc; ++i) {
        // render
        auto c = dx::clients::get("-ex" + std::to_string(i));
        auto _handler = c->getEngine();
        _handler->loadGL();
        _handler->updateView();
        _handler->updateRenderer();
        _handler->render();
        auto buffer = _handler->copyRenderedImage();
        // save
        QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
        std::string filename = "image" + c->getId() + ".PNG";
        img.save(filename.c_str(), nullptr, -1);
        std::cout << "save file as " << filename << std::endl;
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
