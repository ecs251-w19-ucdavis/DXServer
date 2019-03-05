//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Engine.h"

#include "Serializer/Dictionary.h"
#include "Serializer/SceneDictionary.h"
#include "Serializer/VectorSerializer.h"
#include "Serializer/VolumeSerializer.h"

#include "Scene/Geometry/RegularGridVolumeGL.h"

#include "Loader/RegularGridRawBinary.h"
#include "Loader/TetraGridRawBinary.h"

#include "Util/Log.h"
#include "Util/Material.h"
#include "Renderer/GeometryProperty.h"

using v3d::JsonValue;
using v3d::JsonParser;

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace details
{

Engine::Engine(fbo_t fbo, int w, int h)
    : _size{w, h}
    , _fbo{std::move(fbo)}
{}

/**
 * This function will open and parse the JSON configuration file from disk
 * @param project_name The name of the JSON configuration file
 */
void Engine::loadJSONFile(std::string pname)
{
    // fix namespaces
    using namespace v3d::serializer;
    using namespace v3d::dx::serializer;

    // update local variable & and load the file
    JsonParser().load(pname, _jsonRoot);
    _jsonFileName = pname; // for backup purpose

    // load the data source if present
    // @note hard error will be thrown if there is no data specified
    if (_jsonRoot.contains(DATA_SOURCE)) {
        _jsonData = _jsonRoot[DATA_SOURCE];
    } else {
        throw std::runtime_error("[Error] thr project JSON does not contain a data specification, please fix this.");
    }

    // load the view if present
    if (_jsonRoot.contains(VIEW)) {
        _jsonView   = _jsonRoot[VIEW];
        _jsonMethod = _jsonView.get(METHOD_, "").toString();
    } else {
        log() << "[Warn] thr project JSON does not contain a view specification." << std::endl;
    }
}

void Engine::initData()
{
    // setup namespace
    using namespace v3d::serializer;
    using namespace v3d::dx::serializer;

    // analysis json file
    if (_jsonData.isNull()) {
        throw std::runtime_error("[Error] empty json node");
    } else {
        const JsonValue &jsonArray = _jsonData;
        if (!jsonArray.isArray()) {
            throw std::runtime_error("[error] In TetMesh::loadV3d(): Bad JSON format, DATA_SOURCE should be an array.");
        }
        for (int i = 0; i < jsonArray.size(); ++i) {
            const JsonValue &jsonData = jsonArray[i];
            if (!jsonData.contains(ID)) {
                std::cout << "[warning] In TetMesh::loadV3d(): Bad JSON format, DATA_ID does not exist." << std::endl;
            }
            std::string format = jsonData.get(FORMAT, "").toString();
            if (format == FOLDER) {
                PING; // TODO
            } else if (format == MULTIVARIATE) {
                PING; // TODO
            } else if (format == TIME_VARYING) {
                PING; // TODO
            } else if (format == REGULAR_GRID_RAW_BINARY) {
                _mode = REGULAR_GRID;
                _data = v3d::load::RegularGridRawBinary(jsonData, _jsonFileName);
#ifdef  V3D_USE_PVM
            } else if (format == REGULAR_GRID_PVM) {
                PING; // TODO
#endif//V3D_USE_PVM
            } else if (format == TETRAHEDRAL_GRID_RAW_BINARY) {
                _mode = TETRA_GRID;
                _data = v3d::load::TetraGridRawBinaryData(jsonData, _jsonFileName);
            } else if (format == TETRAHEDRAL_GRID_FAST) {
                PING; // TODO
            } else {
                throw std::runtime_error("[error] In TetMesh::loadV3d(): Unsupported Data Format " + format);
            }
        }
    }
}

/**
 * @note: we need an initialization because we will frequently update our scene file
 */
void Engine::initScene()
{
    // now we create transfer functions
    _ctf = std::move(TransferFunction::fromRainbowMap());
    _otf = std::make_shared<OcclusionTransferFunction>();

    // create camera
    _camera = std::make_shared<Camera>();

    // now we create volume and scene
    if (_jsonMethod == "REGULAR_GRID_VOLUME_RAY_CASTING") {

        auto volume = std::make_shared<v3d::RegularGridVolumeGL>();
        auto data = std::dynamic_pointer_cast<RegularGridDataGL>(_data);
        {
            // configure volume
            volume->setData(data);
            vec3 gridO = data->origin();
            vec3 gridS = data->spacing();
            vec3 gridD = vec3(data->dimensions() - ivec3(1)) * gridS;
            // omit the outer-most half voxel
            volume->setTextureBox(Box<float>(gridO - gridS * 0.5f, gridO + gridD + gridS * 0.5f));
            volume->setBoundingBox(Box<float>(gridO, gridO + gridD));
            volume->setClippingBox(volume->boundingBox());
            volume->setDataDirty(true);
            // setup tfn
            volume->setTransferFunction(_ctf);
            volume->setTransferFunction2D(_otf);
            // data range
            switch (data->type()) {
                case V3D_UNSIGNED_BYTE:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<float>()));
                    break;
                case V3D_BYTE:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<char>()));
                    break;
                case V3D_UNSIGNED_SHORT:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<unsigned short>()));
                    break;
                case V3D_SHORT:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<short>()));
                    break;
                case V3D_UNSIGNED_INT:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<unsigned int>()));
                    break;
                case V3D_INT:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<int>()));
                    break;
                case V3D_FLOAT:
                    volume->setScalarMappingRange(dvec2(data->getScalarRange<float>()));
                    break;
                case V3D_DOUBLE:
                    volume->setScalarMappingRange(data->getScalarRange<double>());
                    break;
                default:
                    throw std::runtime_error("[Error] unknown volume value range type");
            }
            // others
            const vec3 gridOrigin = data->origin();
            const vec3 gridSpacing = data->spacing();
            const vec3 gridExtent = vec3(data->dimensions() - ivec3(1)) * gridSpacing;
            float voxelSize = length(gridSpacing) / std::sqrt(3.0f);
            volume->setSampleDistance(voxelSize / 4.0f);
            volume->setOpacityUnitDistance(voxelSize);
            // slices
            vec3 boxCenter = gridOrigin + gridExtent * 0.5f;
            volume->setXSlicePosition(boxCenter.x);
            volume->setYSlicePosition(boxCenter.y);
            volume->setZSlicePosition(boxCenter.z);
        }
        _volume = volume;

        // setup camera
        dvec3 gridOrigin = (data != nullptr) ? dvec3(data->origin()) : dvec3(0.0);
        dvec3 gridSpacing = (data != nullptr) ? dvec3(data->spacing()) : dvec3(1.0);
        dvec3 gridExtent = (data != nullptr) ? (dvec3(data->dimensions() - ivec3(1)) * gridSpacing) : dvec3(1.0);
        _camera->lookAt(gridOrigin + gridExtent * dvec3(0.5, 0.5, 3.0),
                        gridOrigin + gridExtent * dvec3(0.5, 0.5, 0.5),
                        dvec3(0.0, 1.0, 0.0));
        double maxDim = max(max(gridExtent.x, gridExtent.y), gridExtent.z);
        _camera->perspective(45.0, _camera->aspect(), maxDim * 0.01, maxDim * 10.0);

        // create the scene
        _sceneGrid = std::make_shared<RegularGridSceneGL>();
        _sceneGrid->setVolume(volume);
        _sceneGrid->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
        _sceneGrid->setTFPreIntegration(false);
        _sceneGrid->setLighting(true);
        _sceneGrid->setEmptySpaceSkipping(false);
        _sceneGrid->setCamera(_camera);

        // setup renderer
        _rendererGrid = std::make_shared<RegularGridPipelineGL>();
        _rendererGrid->setFramebufferObject(_fbo->sharedFramebufferObject());
        _rendererGrid->setScene(_sceneGrid);

    } else if (_jsonMethod == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING") {

        auto volume = std::make_shared<TetraGridVolumeGL>();
        auto data = std::dynamic_pointer_cast<TetraGridDataGL>(_data);

        // configure volume
        volume->setData(data);
        volume->setBoundingBox(data->grid()->getBoundingBox());
        vec3 boxDim = volume->boundingBox().size();
        vec3 boxCenter = volume->boundingBox().center();
        double maxDim = max(max(boxDim.x, boxDim.y), boxDim.z);

        std::cout << "[debug] "
                  << "boxMin = " << volume->boundingBox().minimum() << ", "
                  << "boxMax = " << volume->boundingBox().maximum()
                  << std::endl;

        dvec2 range(data->getScalarRange<float>());

        // transfer function
        _ctf = std::move(TransferFunction::fromRainbowMap());
        volume->setTransferFunction(_ctf);

        // sampling properties
        volume->setSamplesPerCell(2);
        volume->setSampleDistance(float(maxDim / 256.0));
        volume->setScalarMappingRange(range);
        volume->setOpacityUnitDistance(float(maxDim / 256.0));

        // finalize volume
        _volume = volume;

        // create a scene
        _sceneTets = std::make_shared<TetraGridSceneGL>();
        _sceneTets->setVolume(volume);
        _sceneTets->setTFPreIntegration(true);

        // boundary geometry property ?
        Box<float> boundaryClippingBox(volume->boundingBox().minimum() - boxDim * 0.01f,
                                       volume->boundingBox().maximum() + boxDim * 0.01f);
        auto boundaryGeometryProperty = std::make_shared<GeometryProperty>();
        boundaryGeometryProperty->setVisible(false);
        Material geomMat;
        geomMat.setAmbient(vec4(0.2f, 0.2f, 0.2f, 1.0f));
        geomMat.setDiffuse(vec4(0.8f, 0.8f, 0.8f, 1.0f));
        boundaryGeometryProperty->setFrontMaterial(geomMat);
        boundaryGeometryProperty->setClippingBox(boundaryClippingBox);

        // set camera
        _camera->setNear(maxDim * 0.001);
        _camera->setFar(maxDim * 10.0);
        _camera->lookAt(dvec3(boxCenter) + dvec3(boxDim) * dvec3(0.0, 0.0, 2.5),
                        dvec3(boxCenter),
                        dvec3(0.0, 1.0, 0.0));
        _sceneTets->setCamera(_camera);

        // set renderer
        _rendererTets = std::make_shared<TetraGridPipelineGL>();
        _rendererTets->setFramebufferObject(_fbo->sharedFramebufferObject());
        _rendererTets->setScene(_sceneTets);
        _rendererTets->setBoundaryGeometryProperty(boundaryGeometryProperty);

    } else {
        throw std::runtime_error("[Error] unknown volume type");
    }
}

void Engine::updateView(const v3d::JsonValue &input)
{
    // setup namespace
    using namespace v3d::serializer;
    using namespace v3d::dx::serializer;

    // if view is null, we use the default view value instead
    const JsonValue &view = input.isNull() ? _jsonView : input;
    _jsonMethod = view.get(METHOD_, "").toString();

    // load transfer functions
    updateTransferFunction(view);

    // load camera
    updateCamera(view);

    // load volume
    if (_jsonMethod == "REGULAR_GRID_VOLUME_RAY_CASTING") {
        auto volume = std::dynamic_pointer_cast<v3d::RegularGridVolumeGL>(_volume);
        if (view.contains(VOLUME)) fromJson(view[VOLUME], *volume);
        fromJson(view, *_sceneGrid);
    } else if (_jsonMethod == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING") {
        auto volume = std::dynamic_pointer_cast<v3d::TetraGridVolumeGL>(_volume);
        if (view.contains(VOLUME)) fromJson(view[VOLUME], *volume);
        fromJson(view, *_sceneTets);
    } else {
        throw std::runtime_error("[Error] unknown volume type");
    }
}

void Engine::updateRenderer()
{
    if (_rendererGrid) {
        _rendererGrid->resize(_size.x, _size.y);
    }
    else if (_rendererTets) {
        _rendererTets->resize(_size.x, _size.y);
    }
}

//void Engine::resize(int w, int h)
//{
//  _size.x = w;
//  _size.y = h;
//  updateRenderer();
//}

void Engine::render()
{
    glFinish();
    if (_rendererGrid) {
        _rendererGrid->render();
    }
    else if (_rendererTets) {
        _rendererTets->render();
    }
    glFinish();
}

std::shared_ptr<std::vector<uint8_t>> Engine::copyRenderedImage(bool fix_alpha) const
{
    auto buffer = std::make_shared<std::vector<uint8_t>>(size_t(_size.x) * size_t(_size.y) * size_t(4));
    GLuint currFbo = GLFramebufferObject::currentDrawBinding();
    _fbo->bind();
    V3D_GL_PRINT_ERRORS();
    GLint readBuffer;
    glGetIntegerv(GL_READ_BUFFER, &readBuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, _size.x, _size.y, GL_BGRA, GL_UNSIGNED_BYTE, &(*buffer)[0]);
    glReadBuffer(GLenum(readBuffer));
    V3D_GL_PRINT_ERRORS();
    GLFramebufferObject::bind(currFbo);
    V3D_GL_PRINT_ERRORS();
    if (fix_alpha) {
        for (int i = 0; i < _size.x * _size.y; i++) (*buffer)[i * 4 + 3] = 255;
    }
    return std::move(buffer);
}

///////////////////////////////////////////////////////////////////////////////

void Engine::loadGL()
{
    if (_mode == REGULAR_GRID) {
        std::dynamic_pointer_cast<RegularGridDataGL>(_data)->loadGL();
    } else if (_mode == TETRA_GRID) {
        std::dynamic_pointer_cast<TetraGridGL>(_data)->loadGL();
        std::dynamic_pointer_cast<TetraGridDataGL>(_data)->loadGL();
    } else {
        throw std::runtime_error("[Error] unknown volume type");
    }
}

void Engine::unloadGL()
{
    if (_mode == REGULAR_GRID) {
        std::dynamic_pointer_cast<RegularGridDataGL>(_data)->unloadGL();
    } else if (_mode == TETRA_GRID) {
        std::dynamic_pointer_cast<TetraGridDataGL>(_data)->unloadGL();
        std::dynamic_pointer_cast<TetraGridGL>(_data)->unloadGL();
    } else {
        throw std::runtime_error("[Error] unknown volume type");
    }
}


void Engine::updateTransferFunction(const v3d::JsonValue &view)
{
    // setup namespace
    using namespace v3d::serializer;
    using namespace v3d::dx::serializer;
    // load TFN
    if (view.contains(VOLUME)) {
        const auto &jsonVol = view[VOLUME];
        if (_jsonMethod == "REGULAR_GRID_VOLUME_RAY_CASTING") {
            auto volume = std::dynamic_pointer_cast<RegularGridVolumeGL>(_volume);
            if (jsonVol.contains(TRANSFER_FUNCTION)) {
                *_ctf = fromJson<TransferFunction>(jsonVol[TRANSFER_FUNCTION]);
                _ctf->updateColorMap();
                volume->setTransferFunctionDirty(true);
                volume->setTransferFunctionDirtyCoarse(true);
            }
            if (jsonVol.contains(OCCLUSION_TRANSFER_FUNCTION)) {
                *_otf = fromJson<OcclusionTransferFunction>(jsonVol[OCCLUSION_TRANSFER_FUNCTION]);
                _otf->update();
                volume->setTransferFunction2DDirty(true);
            }
        }
        else if (_jsonMethod == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING") {
            auto volume = std::dynamic_pointer_cast<TetraGridVolumeGL>(_volume);
            if (jsonVol.contains(TRANSFER_FUNCTION)) {
                *_ctf = fromJson<TransferFunction>(jsonVol[TRANSFER_FUNCTION]);
                _ctf->updateColorMap();
            }
        }
        else {
            throw std::runtime_error("[Error] unknown volume type");
        }
    }
}

void Engine::updateCamera(const v3d::JsonValue &view)
{
    // setup namespace
    using namespace v3d::serializer;
    using namespace v3d::dx::serializer;
    // load camera
    if (view.contains(CAMERA)) {
        *_camera = fromJson<Camera>(view[CAMERA]);
        _camera->setAspect(_size.x / static_cast<float>(_size.y));
    }
}

}

///////////////////////////////////////////////////////////////////////////////

}}
