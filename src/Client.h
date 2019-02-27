#pragma once
#ifndef DXSERVER_CLIENT_H
#define DXSERVER_CLIENT_H

#include "SceneHandler.h"
#include "Renderer/FramebufferGL.h"
#include <string>
#include <queue>

namespace v3d { namespace dx {

class Client {
public:
	void Init(const char *_filename, int _id)
	{
		filename = _filename;
		id = _id;
	}
	// void ParseRequest(int request);
	void RenderScene(std::shared_ptr<FramebufferGL> fbo);
private:
	// bool 
	const char *filename;
	int id;
};

// class ClientQueue {
// public:
// private:
// 	queue<Client>
// };
}


}

#endif //DXSERVER_CLIENT_H