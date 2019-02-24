#include "SceneManager.h"

#define MULTI_CLIENT_MODE 1

namespace v3d {

void createScene(int *argc, const char **argv, std::shared_ptr<FramebufferGL> fbo)
{
#if MULTI_CLIENT_MODE

    dx::Client Clients[2];
    Clients[0].Init(argv[1], 0);
    Clients[1].Init(argv[2], 1);
    Clients[0].RenderScene(fbo);
    Clients[1].RenderScene(fbo);

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
