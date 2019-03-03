//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Client.h"

#include "Graphics/DXGL.h"

#include <vector>
#include <memory>

using namespace v3d::dx;

// TODO we need a lock
static std::vector<api::client_t> client_queue;

void details::Client::init(const std::string& fname, int w, int h)
{
	_fbo = std::make_shared<FramebufferGL>(w, h);
    _handler = std::make_shared<Engine>(fname, _fbo, w, h);
}

void details::Client::render()
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

api::client_t clientlist::append() {
	auto id = static_cast<api::client_id_t>(client_queue.size());
	auto client = std::make_shared<details::Client>();
	client->setId(id);
	client_queue.push_back(client);
	return client;
}

int clientlist::remove(api::client_id_t)
{
	PING;
	return 0;
}

api::client_t clientlist::get(api::client_id_t id)
{
	return client_queue[id];
}