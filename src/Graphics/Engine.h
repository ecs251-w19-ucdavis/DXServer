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

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation details. Usually we should not directly access a class inside
 * a details namespace. We should either use a corresponding shared pointer
 * creater, or a list manipulator.
 */
namespace details {

/**
 * This is an internal class for rendering
 */
class Engine {
    using V3DMedium   = v3d::api::V3DMedium;
    using V3DGeometry = v3d::api::V3DGeometry;
    using fbo_t = std::shared_ptr<FramebufferGL>;
    enum  mode_t { INVALID, REGULAR_GRID, TETRA_GRID };
public:
    /**
     * Constructor
     * @param fbo We always need a framebuffer object for drawing
     * @param w The initial framebuffer width
     * @param h The initial framebuffer height
     */
    Engine(fbo_t fbo, int w, int h);

    /**
     * This function will open and parse the JSON configuration file from disk
     * @param project_name The name of the JSON configuration file
     */
    void loadJSONFile(std::string pname);

    JsonValue serializeScene();

    void loadData();
    void initScene();

    void updateView(const JsonValue &input = JsonValue());
    void updateRenderer();
    void render();
    std::shared_ptr<std::vector<uint8_t>> copyRenderedImage(bool fix_alpha = true) const;
    std::string encodeRenderedImage(bool fix_alpha = true) const;

    void resize(int w, int h);

    void loadGL();
    void unloadGL();

protected:

    void serializeTF(JsonValue& json) const;

    void updateCamera(const JsonValue &);
    void updateTransferFunction(const JsonValue &);
    std::shared_ptr<RegularGridSceneGL> getSceneGrid() { return _sceneGrid; }
    std::shared_ptr<TetraGridSceneGL>   getSceneTets() { return _sceneTets; }
    std::shared_ptr<RegularGridPipelineGL> getRendererGrid() { return _rendererGrid; }
    std::shared_ptr<TetraGridPipelineGL>   getRendererTets() { return _rendererTets; }

protected:
    ivec2 _size;
    fbo_t _fbo;
    mode_t _mode = INVALID;

    std::string _jsonFileName;
    JsonValue   _jsonRoot;
    JsonValue   _jsonData;
    JsonValue   _jsonView;
    std::string _jsonMethod;

    V3DMedium   _data;
    V3DGeometry _volume;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<TransferFunction> _ctf;
    std::shared_ptr<OcclusionTransferFunction> _otf;

    // TODO things below need to be refactored
    std::shared_ptr<RegularGridSceneGL>    _sceneGrid;
    std::shared_ptr<TetraGridSceneGL>      _sceneTets;
    std::shared_ptr<RegularGridPipelineGL> _rendererGrid;
    std::shared_ptr<TetraGridPipelineGL>   _rendererTets;
};

}

///////////////////////////////////////////////////////////////////////////////

// we do not expose anything in the dx namespace
using engine_t = std::shared_ptr<details::Engine>;

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_SCENELOADER_H
