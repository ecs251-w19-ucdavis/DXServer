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

static std::map<clid_t, client_t> _client_queue;
static std::mutex                 _client_mutex; // lock for the client queue

void clients::lock()
{
	_client_mutex.lock();
}

void clients::unlock()
{
	_client_mutex.unlock();
}

int clients::pop(clid_t id)
{
	_client_mutex.lock();
	{
		auto it = _client_queue.find(id);
		if (it != _client_queue.end()) { _client_queue.erase(it); }
	}
	_client_mutex.unlock();
	return 0;
}

client_t clients::add(clid_t id)
{
	client_t client;
	_client_mutex.lock();
	{
		client.reset(new details::Client());
		client->setId(id);
		_client_queue[id] = client;
	}
	_client_mutex.unlock();
	return client;
}

client_t clients::get(clid_t id)
{
	client_t ret(nullptr);
	_client_mutex.lock();
	{
		auto it = _client_queue.find(id);
		if (it != _client_queue.end()) {
			ret = it->second;
		}
	}
	_client_mutex.unlock();
	return ret;
}

bool clients::has(clid_t id)
{
	bool ret = false;
	_client_mutex.lock();
	{
		auto it = _client_queue.find(id);
		if (it != _client_queue.end()) {
			ret = true;
		}
	}
	_client_mutex.unlock();
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
    if (!_created) {
		_fbo = std::make_shared<FramebufferGL>(w, h);
		_handler = std::make_shared<Engine>(_fbo, w, h);
		_created = true;
    }
}

void details::Client::initGL()
{
	_handler->loadGL();
    _handler->initScene();
	_handler->updateView();
	_handler->unloadGL();
	_ready = true;
}

void details::Client::openProject(const std::string& fname)
{
	_handler->loadJSONFile(fname);
    _handler->loadData();
}

//void details::Client::loadDataToGPU() // GPU
//{
//	// do nothing
//	PING;
//}

void details::Client::removeDataFromGPU() // GPU
{
//	_handler->unloadGL();
	PING;
}

void details::Client::closeProject()
{
	// do nothing
	PING;
}

json_t details::Client::renderFrame(const JsonValue &input)
{
	JsonValue params;
	if (_ready) {
		_handler->loadGL();
		_handler->updateView(input);
		_handler->updateRenderer();
		_handler->render();
		params["data"] = "data:image/jpeg;base64," + _handler->encodeRenderedImage();
		_handler->unloadGL();
		return params;
	} else {
		params["data"] = "";
		return params;
	}
}

json_t details::Client::getScene()
{
//	_currentProjectName = "/Users/qwu/Work/projects/vidi/dxserver/data/vorts1.json";
//	_fbo = std::make_shared<FramebufferGL>(winW, winH);
//	_handler = std::make_shared<Engine>(_fbo, winW, winH);
//	_handler->loadJSONFile(_currentProjectName);
//	_handler->loadData();
//	_handler->initScene();
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
	if (_created)
	{
		std::cout << "you initialized the client twice" << std::endl;
	}

	_currentProjectName = fname;

	_fbo = std::make_shared<FramebufferGL>(w, h);

	_handler = std::make_shared<Engine>(_fbo, w, h);

	_created = true;
}

void details::Client::renderDebug()
{
	// create renderer
	_handler->loadJSONFile(_currentProjectName);

    _handler->loadData();

	//_handler->loadGL();

    _handler->initScene();
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
