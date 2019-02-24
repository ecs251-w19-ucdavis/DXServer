#pragma once
#ifndef DXSERVER_CLIENT_H
#define DXSERVER_CLIENT_H

#include "SceneHandler.h"
#include "Renderer/FramebufferGL.h"
#include <string>

namespace v3d { namespace dx {

class Client {
public:
	void Init(const char *_filename, int _id)
	{
		filename = _filename;
		id = _id;
	}
	void RenderScene(std::shared_ptr<FramebufferGL> fbo);
private:
	const char *filename;
	int id;
};

}}

#endif //DXSERVER_CLIENT_H