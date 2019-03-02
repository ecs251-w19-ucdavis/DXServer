#include "Client.h"

#include "Graphics/DXGL.h"



// void Client::ParseRequest(int request)
// {
// 	switch(request) {
// 		case 0: // initialize
// 			PING;
// 	}
// }

void v3d::dx::Client::Init(const std::string& fname, size_t client_id)
{
    _handler
}

void v3d::dx::Client::RenderScene(std::shared_ptr<FramebufferGL> fbo)
{

	// create renderer
	v3d::dx::SceneHandler handler(filename, std::move(fbo), dx::winW, dx::winH);
	handler.initData();
	handler.initScene();
	handler.updateView();
	handler.updateRenderer();

	// render
	handler.render();

	// get framebuffer image
	auto buffer = handler.copyRenderedImage();

	// save
	QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
	std::string filename = "image" + std::to_string(id) + ".PNG";
	img.save(filename.c_str(), 0, -1);
	std::cout << "save file as " << filename << std::endl;
}
