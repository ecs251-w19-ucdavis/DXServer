//
// Created by Qi Wu on 2019-02-15.
//

#pragma once
#ifndef DXSERVER_SCENELOADER_H
#define DXSERVER_SCENELOADER_H

#include "Util/JsonParser.h"

#include "Scene/IGeometry.h"
#include "Scene/IMedium.h"

#include "Scene/TransferFunction/TransferFunction.h"
#include "Scene/TransferFunction/OcclusionTransferFunction.h"

#include "Renderer/RegularGridSceneGL.h"
#include "Renderer/TetraGridSceneGL.h"

#include "Util/Camera.h"

#include <string>

using v3d::api::V3DMedium;
using v3d::api::V3DGeometry;

namespace v3d { namespace dx {

class SceneLoader {
public:
    explicit SceneLoader(std::string filename, int W, int H);
    void initData();
    void initScene();
    void updateView(const JsonValue& input = JsonValue());
    void updateCamera(const JsonValue&);
    void updateTransferFunction(const JsonValue&);

    std::shared_ptr<RegularGridSceneGL> getSceneGrid() { return _sceneGrid; }
    std::shared_ptr<TetraGridSceneGL>   getSceneTets() { return _sceneTets; }

protected:
    ivec2 size;

    std::string _method;

    std::string _jsonFileName;
    JsonValue   _jsonRoot;
    JsonValue   _jsonData;
    JsonValue   _jsonView;

    V3DMedium   _data;
    V3DGeometry _volume;

    std::shared_ptr<Camera> _camera;

    std::shared_ptr<TransferFunction>          _ctf;
    std::shared_ptr<OcclusionTransferFunction> _otf;

    std::shared_ptr<RegularGridSceneGL> _sceneGrid; // TODO this is a hack! Eventually they should be child classes of
    std::shared_ptr<TetraGridSceneGL>   _sceneTets; // TODO   one common parent
};

}
}

#endif //DXSERVER_SCENELOADER_H
