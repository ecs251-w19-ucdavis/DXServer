#pragma once
#ifndef DXSERVER_CLIENT_H
#define DXSERVER_CLIENT_H

#include "Engine.h"
#include "Renderer/FramebufferGL.h"

#include <QImage>

#include <string>
//#include <queue>
//#include <atomic>

namespace v3d {
namespace dx {

class Client {
public:
    Client() = default;
    void setId(int64_t id) { _id = id; }
	void init(const std::string& fname, int w, int h);
	void render();
private:
    int64_t _id = -1;
    std::shared_ptr<FramebufferGL> _fbo;
    std::shared_ptr<dx::Engine> _handler;
};

}
}

#endif //DXSERVER_CLIENT_H