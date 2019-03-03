#include "Client.h"

#include "Graphics/DXGL.h"

void v3d::dx::Client::init(const std::string& fname, int w, int h)
{
	_fbo = std::make_shared<FramebufferGL>(w, h);
    _handler = std::make_shared<Engine>(fname, _fbo, w, h);
}

void v3d::dx::Client::render()
{
	// create renderer
	_handler->initData();
	_handler->initScene();
	_handler->updateView();
	_handler->updateRenderer();

	// render
	_handler->render();

	// get framebuffer image
	auto buffer = _handler->copyRenderedImage();

	// save
	QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
	std::string filename = "image" + std::to_string(_id) + ".PNG";
	img.save(filename.c_str(), nullptr, -1);
	std::cout << "save file as " << filename << std::endl;
}
