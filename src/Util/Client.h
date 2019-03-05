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

#include "Renderer/FramebufferGL.h"
#include "Graphics/Engine.h"

#include <QImage>

#include <string>
#include <memory>
#include <atomic>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace details { class Client; }

using clid_t = int;
using client_t = std::shared_ptr<details::Client>;

// here we add handlers for append, retrieve, modify clients
namespace clients {

/** @return value 1 means okay, value 0 means error */
int /* err */ remove(clid_t);

/** @return nullptr means error */
client_t add(clid_t);

/** @return nullptr means error */
client_t get(clid_t);

/** check if this client exists */
bool has(clid_t);

}

///////////////////////////////////////////////////////////////////////////////

// detailed implementation
namespace details {
/**
 * This class defines the behavior of each client
 */
class Client {
	friend client_t clients::add(clid_t);
public:

	void   setId(clid_t id) { _id = id; }
	clid_t getId() const { return _id; }

	/**
     * @note Can be read by RequestQueues
     * @return counter value
     */
	size_t currCounterValue();

	/**
	 * Increment the next request counter and return the value before
	 * @note Can be read by RequestQueues
	 * @return
	 */
	size_t nextCounterValue();

	/**
     * @note Be incremented by RequestHandler
     */
	void incrementCurrCounter();


	void init(int w, int h);

//	void loadGL();
//	void unloadGL();

    void openProject(const std::string& fname)
    {
		_handler->loadJSONFile(_currentProjectName);
    }

    void loadDataToGPU() // GPU
    {
		// do nothing
    }

	void removeDataFromGPU() // GPU
	{
		_handler->unloadGL();
	}

	void closeProject()
	{
    	// do nothing
	}

	void getScene() // GPU
	{
		_handler->loadGL();
	}

	void renderFrame()
	{

	}



	void initDebug(const std::string& fname, int w, int h);
	void renderDebug();


private:
	Client() : _curr_request_counter{0}, _next_request_counter{0} {}

private:

    std::string _currentProjectName;

	bool _initialized = false;

//	std::mutex _lock;
//	size_t _curr_request_counter = 0;
//	size_t _next_request_counter = 0;

    std::atomic<size_t> _curr_request_counter;
    std::atomic<size_t> _next_request_counter;

	clid_t _id = 0; // id == 0 means invalid
	std::shared_ptr<FramebufferGL> _fbo;
	std::shared_ptr<dx::details::Engine> _handler;
};
}

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_CLIENT_H
