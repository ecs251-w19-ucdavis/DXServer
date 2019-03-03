#include "Workspace.h"

#include "Client.h"
#include "Graphics/DXGL.h"

#include <string>

#define MULTI_CLIENT_MODE 1

namespace v3d {

void startWorkspace(int *argc, const char **argv)
{
#if MULTI_CLIENT_MODE

    dx::Client clients[2];
    clients[0].setId(0);
    clients[1].setId(1);
    clients[0].init(argv[1], dx::winW, dx::winH);
    clients[1].init(argv[2], dx::winW, dx::winH);
    clients[0].render();
    clients[1].render();

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
