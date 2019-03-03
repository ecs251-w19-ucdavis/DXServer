//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_SCENELOADER_H
#define DXSERVER_SCENELOADER_H

#include "Util/JsonParser.h"

#include "Scene/IGeometry.h"
#include "Scene/IMedium.h"

#include "Util/Camera.h"
#include "Scene/RegularGridSceneGL.h"
#include "Scene/TetraGridSceneGL.h"
#include "Scene/TransferFunction/TransferFunction.h"
#include "Scene/TransferFunction/OcclusionTransferFunction.h"
#include "Engine/RegularGridPipelineGL.h"
#include "Engine/TetraGridPipelineGL.h"
#include "Renderer/FramebufferGL.h"

#include <string>

namespace v3d {
namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace details {

/**
 * This is an internal class for rendering
 */
class Engine {
    using V3DMedium = v3d::api::V3DMedium;
    using V3DGeometry = v3d::api::V3DGeometry;
    using fbo_t = std::shared_ptr<FramebufferGL>;
public:
    explicit Engine(std::string project_name, fbo_t framebuffer_object, int initial_width, int initial_height);
    void initData();
    void initScene();
    void updateView(const JsonValue &input = JsonValue());
    void updateRenderer();
    void render();
    std::shared_ptr<std::vector<uint8_t>> copyRenderedImage(bool fix_alpha = true) const;

//    void resize(int w, int h); // TODO I am not sure we need to resize framebuffer in code
//    void updateFBO(fbo_t fbo) { _fbo = std::move(fbo); }; // enable it if we really need to

protected:
    void updateCamera(const JsonValue &);
    void updateTransferFunction(const JsonValue &);
    std::shared_ptr<RegularGridSceneGL> getSceneGrid()
    { return _sceneGrid; }
    std::shared_ptr<TetraGridSceneGL> getSceneTets()
    { return _sceneTets; }
    std::shared_ptr<RegularGridPipelineGL> getRendererGrid()
    { return _rendererGrid; }
    std::shared_ptr<TetraGridPipelineGL> getRendererTets()
    { return _rendererTets; }

protected:
    ivec2 _size;
    fbo_t _fbo;

    std::string _jsonFileName;
    JsonValue _jsonRoot;
    JsonValue _jsonData;
    JsonValue _jsonView;
    std::string _jsonViewMethod;

    V3DMedium _data;
    V3DGeometry _volume;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<TransferFunction> _ctf;
    std::shared_ptr<OcclusionTransferFunction> _otf;

    // TODO things below need to be refactored
    std::shared_ptr<RegularGridSceneGL> _sceneGrid; // TODO this is a hack! Eventually they should be child classes of
    std::shared_ptr<TetraGridSceneGL> _sceneTets; // TODO   one common parent
    std::shared_ptr<RegularGridPipelineGL> _rendererGrid;
    std::shared_ptr<TetraGridPipelineGL> _rendererTets;
};

}

///////////////////////////////////////////////////////////////////////////////

// we do not expose anything in the api namespace
namespace api {
using engine_t = std::shared_ptr<details::Engine>;
}

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_SCENELOADER_H
