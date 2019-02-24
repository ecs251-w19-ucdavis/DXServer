#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <QImage>
#include <QFile>

#include "SceneHandler.h"

#include "Util/Library.h"
//#include "Util/JsonParser.h"
#include "Engine/IRenderer.h"

//#include "Data/RegularGridLoader.h"
//#include "Scene/Medium/RegularGridDataGL.h"
//#include "Scene/Geometry/RegularGridVolumeGL.h"
#include "Scene/RegularGridSceneGL.h"
#include "Engine/RegularGridPipelineGL.h"
#include "Renderer/FramebufferGL.h"
//#include "Util/Camera.h"
#include "Util/SourceCodeManager.h"

#include "DXGL.h"

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