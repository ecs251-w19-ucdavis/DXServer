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
namespace api {
using client_id_t = size_t;
using client_t = std::shared_ptr<details::Client>;
}

// here we add handlers for append, retrieve, modify clients
namespace clients {
/** @return 1 means okay, 0 means error */
int /* err */ remove(api::client_id_t);
/** @return nullptr means error */
api::client_t append();
/** @return nullptr means error */
api::client_t get(api::client_id_t);
}

///////////////////////////////////////////////////////////////////////////////

// detailed implementation
namespace details {
/**
 * This class defines the behavior of each client
 */
class Client {
public:
	Client() = default;
	void setId(api::client_id_t id) { _id = id; }
	api::client_id_t getId() const { return _id; }
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
	bool initialized = false;
	api::client_id_t _id = 0; // id == 0 means invalid
	std::shared_ptr<FramebufferGL> _fbo;
	std::shared_ptr<dx::details::Engine> _handler;
};
}

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_CLIENT_H
