#pragma once
#ifndef DXSERVER_CLIENT_H
#define DXSERVER_CLIENT_H

#include "SceneHandler.h"
#include "Renderer/FramebufferGL.h"

#include <QImage>

#include <string>
#include <queue>
#include <atomic>

namespace v3d {
namespace dx {

class Client {
public:
    Client() : _client_id{-1}, _counter(0) {}
	void Init(const std::string& fname, size_t client_id);
	void RenderScene(std::shared_ptr<FramebufferGL> fbo);
private:
    int64_t _client_id;
	std::atomic<size_t> _counter;
    std::shared_ptr<v3d::dx::SceneHandler> _handler;
};

}
}

#endif //DXSERVER_CLIENT_H