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

void details::Client::initGL()
{
	_handler->createFBO();
	_handler->loadGL();
    _handler->initScene();
	_handler->updateView();
	_handler->unloadGL();
	_ready = true;
}

void details::Client::openProject(const std::string& fname, int w, int h)
{
    if (!_created) {
        _handler = std::make_shared<Engine>(w, h);
        _created = true;
    }
	_handler->loadJSONFile(fname);
    _handler->loadData();
}


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
	if (_handler) {
		return std::move(_handler->serializeScene());
	} else {
		return json_t();
	}
}

///////////////////////////////////////////////////////////////////////////////
