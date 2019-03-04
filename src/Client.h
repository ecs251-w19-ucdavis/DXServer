//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_CLIENT_H
#define DXSERVER_CLIENT_H

#include "Engine.h"
#include "Renderer/FramebufferGL.h"

#include <QImage>

#include <string>
#include <memory>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace details { class Client; }

using client_id_t = int;
using client_t = std::shared_ptr<details::Client>;

// here we add handlers for append, retrieve, modify clients
namespace clients {

/** @return value 1 means okay, value 0 means error */
int /* err */ remove(client_id_t);

/** @return nullptr means error */
client_t add(client_id_t);

/** @return nullptr means error */
client_t get(client_id_t);

/** check if this client exists */
bool has(client_id_t);

}

///////////////////////////////////////////////////////////////////////////////

// detailed implementation
namespace details {
/**
 * This class defines the behavior of each client
 */
class Client {
	friend client_t clients::add(client_id_t);
public:

	void        setId(client_id_t id) { _id = id; }
	client_id_t getId() const         { return _id; }

	/**
     * @note Can be read by RequestQueues
     * @return counter value
     */
	size_t currCounterValue()
	{
		_lock.lock();
		size_t v = _curr_request_counter;
		_lock.unlock();
		return v;
	}
	/**
	 * Increment the next request counter and return the value before
	 * @note Can be read by RequestQueues
	 * @return
	 */
	size_t nextCounterValue()
	{
		_lock.lock();
		size_t v = _next_request_counter++;
		_lock.unlock();
		return v;
	}

	/**
     * @note Be incremented by RequestHandler
     */
	void incrementCurrCounter()
	{
		_lock.lock();
		_curr_request_counter += 1;
		_lock.unlock();
	}



	void init(const std::string& fname, int w, int h);
	void loadGL();
	void unloadGL();
	void loadData(const std::string& fname);
	void getScene();
	void render();


	void handleOpenProjectRequested(std::string projFileName, int clientId);
	void handleCloseProjectRequested(int clientId);
	void handleGetSceneRequested(int64_t id, int clientId);
	void handleFrameRequested(const v3d::JsonValue& scene, int clientId);

private:
	Client() = default;

private:
	bool initialized = false;

	std::mutex _lock;
	size_t _curr_request_counter = 0;
	size_t _next_request_counter = 0;

	client_id_t _id = 0; // id == 0 means invalid
	std::shared_ptr<FramebufferGL> _fbo;
	std::shared_ptr<dx::details::Engine> _handler;
};
}

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_CLIENT_H
