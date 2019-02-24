#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "SceneHandler.h"
#include "Renderer/FramebufferGL.h"
#include <string>

namespace v3d{

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
}

#endif