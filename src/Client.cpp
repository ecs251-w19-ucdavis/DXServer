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
#include <mutex>

using namespace v3d::dx;

///////////////////////////////////////////////////////////////////////////////

static std::map<client_id_t, client_t> client_queue;
static std::mutex                                client_mutex;
static client_id_t                          client_next_id = 1;

void details::Client::init(const std::string& fname, int w, int h)
{
	if (initialized)
	{
		PING;
		std::cout << "you initialized the client twice" << std::endl;
	}
	_fbo = std::make_shared<FramebufferGL>(w, h);
	_handler = std::make_shared<Engine>(fname, _fbo, w, h);
	initialized = true;
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

///////////////////////////////////////////////////////////////////////////////

client_t clients::append()
{
	client_t client;
	client_mutex.lock();
	{
		auto id = client_next_id++;
		client = std::make_shared<details::Client>();
		client->setId(id);
		client_queue[id] = client;
	}
	client_mutex.unlock();
	return client;
}

int clients::remove(client_id_t id)
{
	client_mutex.lock();
	{
		auto it = client_queue.find(id);
		if (it != client_queue.end()) { client_queue.erase(it); }
	}
	client_mutex.unlock();
	return 0;
}

client_t clients::get(client_id_t id)
{
	client_t ret(nullptr);
	client_mutex.lock();
	{
		auto it = client_queue.find(id);
		if (it != client_queue.end()) {
			ret = it->second;
		}
	}
	client_mutex.unlock();
	return ret;
}