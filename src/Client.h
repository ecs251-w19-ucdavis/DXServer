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

namespace v3d {
namespace dx {

namespace api { using client_id_t = int64_t; };

// details
namespace details {
/**
 * This class defines the behavior of each client
 */
class Client {
public:
	// the reason to put construct private is that
	Client() = default;
	void setId(api::client_id_t id) { _id = id; }
	api::client_id_t getId() const { return _id; }
	void init(const std::string& fname, int w, int h);
	void render();
private:
	api::client_id_t _id = -1;
	std::shared_ptr<FramebufferGL> _fbo;
	std::shared_ptr<dx::Engine> _handler;
};
}

// namespace for shared pointer
namespace api { using client_t = std::shared_ptr<details::Client>; }

// here we add handlers for append, retrieve, modify clients
namespace clientlist {
int          remove(api::client_id_t);
api::client_t append();
api::client_t get(api::client_id_t);
}



}
}

#endif //DXSERVER_CLIENT_H