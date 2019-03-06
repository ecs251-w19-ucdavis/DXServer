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
#include "Util/Log.h"

#include <vector>
#include <memory>
#include <mutex>

using namespace v3d::dx;

///////////////////////////////////////////////////////////////////////////////

static std::map<clid_t, client_t> client_queue;
static std::mutex                 client_mutex; // lock for the client queue

int clients::pop(clid_t id)
{
	client_mutex.lock();
	{
		auto it = client_queue.find(id);
		if (it != client_queue.end()) { client_queue.erase(it); }
	}
	client_mutex.unlock();
	return 0;
}

client_t clients::add(clid_t id)
{
	client_t client;
	client_mutex.lock();
	{
		client.reset(new details::Client());
		client->setId(id);
		client_queue[id] = client;
	}
	client_mutex.unlock();
	return client;
}

client_t clients::get(clid_t id)
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

bool clients::has(clid_t id)
{
	bool ret = false;
	client_mutex.lock();
	{
		auto it = client_queue.find(id);
		if (it != client_queue.end()) {
			ret = true;
		}
	}
	client_mutex.unlock();
	return ret;
}

///////////////////////////////////////////////////////////////////////////////

size_t details::Client::currCounterValue()
{
	return _curr_request_counter;
}

size_t details::Client::nextCounterValue()
{
	return _next_request_counter++;
}

void details::Client::incrementCurrCounter()
{
	_curr_request_counter += 1;
}

void details::Client::init(int w, int h)
{
    if (_initialized) { log() << "[Warn] you initialized the client twice" << std::endl; }
    _fbo = std::make_shared<FramebufferGL>(w, h);
    _handler = std::make_shared<Engine>(_fbo, w, h);
    _initialized = true;
}

void details::Client::initGL()
{
	_handler->loadGL();
	_handler->updateScene();
	_handler->updateView();
	_handler->unloadGL();
}

void details::Client::openProject(const std::string& fname)
{
	_handler->loadJSONFile(fname);
	_handler->updateData();
}

void details::Client::loadDataToGPU() // GPU
{
	// do nothing
}

void details::Client::removeDataFromGPU() // GPU
{
	_handler->unloadGL();
}

void details::Client::closeProject()
{
	// do nothing
}

void details::Client::renderFrame()
{

}

json_t details::Client::getScene()
{
//	_currentProjectName = "/Users/qwu/Work/projects/vidi/dxserver/data/vorts1.json";
//	_fbo = std::make_shared<FramebufferGL>(winW, winH);
//	_handler = std::make_shared<Engine>(_fbo, winW, winH);
//	_handler->loadJSONFile(_currentProjectName);
//	_handler->updateData();
//	_handler->updateScene();
//	_handler->updateView();
//	_handler->updateRenderer();
	if (_handler) {
		return std::move(_handler->serializeScene());
	} else {
		return json_t();
	}
}

void details::Client::initDebug(const std::string& fname, int w, int h)
{
	if (_initialized)
	{
		std::cout << "you initialized the client twice" << std::endl;
	}

	_currentProjectName = fname;

	_fbo = std::make_shared<FramebufferGL>(w, h);

	_handler = std::make_shared<Engine>(_fbo, w, h);

	_initialized = true;
}

void details::Client::renderDebug()
{
	// create renderer
	_handler->loadJSONFile(_currentProjectName);

    _handler->updateData();

	//_handler->loadGL();

	_handler->updateScene();
	_handler->updateView();
	_handler->updateRenderer();

	//_handler->unloadGL();

	//auto x = _handler->serializeScene();
	//std::cout << JsonParser().stringify(x) << std::endl;

	_handler->loadGL();

	// render
	_handler->render();

	// get framebuffer image
	auto buffer = _handler->copyRenderedImage();

	// save
	QImage img = QImage(&(*buffer)[0], dx::winW, dx::winH, QImage::Format_RGB32).mirrored(false, true);
	std::string filename = "image" + _id + ".PNG";
	img.save(filename.c_str(), nullptr, -1);
	std::cout << "save file as " << filename << std::endl;

	_handler->unloadGL();
}

///////////////////////////////////////////////////////////////////////////////
