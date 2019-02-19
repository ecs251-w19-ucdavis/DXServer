//
// Created by Qi Wu on 2019-02-15.
//

#include "SceneLoader.h"


#include "Serializer/Dictionary.h"
#include "Serializer/SceneDictionary.h"
#include "Serializer/VectorSerializer.h"
#include "Serializer/VolumeSerializer.h"

//#include "Renderer/IRenderer.h"

#include "Data/RegularGridLoader.h"
#include "Scene/Medium/RegularGridDataGL.h"
#include "Scene/Geometry/RegularGridVolumeGL.h"
//#include "Scene/TransferFunction/TransferFunction.h"
//#include "Scene/TransferFunction/OcclusionTransferFunction.h"
//#include "Renderer/RegularGridSceneGL.h"
//#include "Renderer/RegularGridPipelineGL.h"
//#include "Renderer/FramebufferGL.h"

using v3d::JsonValue;
using v3d::JsonParser;

std::string FixSlash(std::string path)
{
#ifdef WIN32
  std::replace(path.begin(), path.end(), '/', '\\');
#else
  std::replace(path.begin(), path.end(), '\\', '/');
#endif
  return path;
}
std::string PathRelToAbs(std::string filename, std::string headername)
{
  if (filename.length() < 2) {
    throw std::runtime_error("[error] In PathRelToAbs(): filename is too short");
  }
  if (filename[0] == '/' || filename[1] == ':') {
    return FixSlash(filename);;
  }
  else {
    size_t found = headername.find_last_of("/\\");
    return FixSlash(headername.substr(0, found) + '\\' + filename);
  }
}
namespace v3d { namespace load {
V3DMedium RegularGridRawBinary(const JsonValue& json, std::string jsonFileName)
{
  // output
  V3DGeometry ret;

  // get common name and id
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;
  int         model_id;   // might be useless
  std::string model_name; // might be useless
  if (json.contains(ID)) {
    model_id = json[ID].toInt();
  }
  if (json.contains(NAME)) {
    model_name = json[NAME].toString();
  }

  // get filename
  std::string model_filename;
  int64_t     model_offset;
  ivec3       model_dimensions;
  Type        model_type;
  Endian      model_endian;
  bool        model_fileul;
  if (json.contains(FILE_NAME)) {
    model_filename = PathRelToAbs(json[FILE_NAME].toString(), jsonFileName);
  } else throw std::runtime_error("[Error] header does not contain grid filename");
  if (json.contains(OFFSET)) {
    model_offset = json[OFFSET].toInt64();
  } else throw std::runtime_error("[Error] header does not contain grid offset");
  if (json.contains(DIMENSIONS)) {
    model_dimensions = fromJson<ivec3>(json[DIMENSIONS]);
  } else throw std::runtime_error("[Error] header does not contain grid dimensions");
  if (json.contains(TYPE)) {
    model_type = typeFromJson(json[TYPE]);
  } else throw std::runtime_error("[Error] header does not contain grid type");
  if (json.contains(ENDIAN)) {
    model_endian = endianFromJson(json[ENDIAN]);
  } else throw std::runtime_error("[Error] header does not contain grid endianness");
  if (json.contains(FILE_UPPER_LEFT)) {
    model_fileul = json[FILE_UPPER_LEFT].toBool();
  } else {
    model_fileul = false;
  }

  // load data
  auto loader = std::make_shared<RegularGridLoader>();
  auto medium = std::make_shared<RegularGridDataGL>();
  loader->setFileName(model_filename);
  loader->setOffset(model_offset);
  loader->setDimensions(model_dimensions);
  loader->setType(model_type);
  loader->setEndian(model_endian);
  loader->setFileUpperLeft(model_fileul);
  loader->setOutputData(medium.get());
  loader->update();
  medium->loadGL();

  // return
  return medium;
}
}}

void v3d::dx::SceneLoader::loadMedium()
{
  // setup namespace
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;
  // analysis json file
  if (_jsonData.isNull()) {
    throw std::runtime_error("[Error] empty json node");
  } else {
    const JsonValue& jsonArray = _jsonData;
    if (!jsonArray.isArray()) {
      throw std::runtime_error("[error] In TetMesh::loadV3d(): Bad JSON format");
    }
    for (int i = 0; i < jsonArray.size(); ++i) {
      const JsonValue& jsonData = jsonArray[i];
      if (!jsonData.contains(ID)) {
        std::cout << "[warning] In TetMesh::loadV3d(): Bad JSON format" << std::endl;
        continue;
      }
      std::string format = jsonData.get(FORMAT, "").toString();
      if (format == FOLDER) {
//        dataSrc = new DataSourceFolder();
      } else if (format == MULTIVARIATE) {
//        dataSrc = new MultivariateDataSource();
      } else if (format == TIME_VARYING) {
//        dataSrc = new TimeVaryingDataSource();
      } else if (format == REGULAR_GRID_RAW_BINARY) {
        _data = v3d::load::RegularGridRawBinary(jsonData, _jsonFileName);
#ifdef V3D_USE_PVM
      } else if (format == REGULAR_GRID_PVM) {
//        dataSrc = new RegularGridPVMData();
#endif // V3D_USE_PVM
      } else if (format == TETRAHEDRAL_GRID_RAW_BINARY) {
//        dataSrc = new TetraGridRawBinaryData();
      } else if (format == TETRAHEDRAL_GRID_FAST) {
//        dataSrc = new TetraGridFASTData();
      }
      else {
        throw std::runtime_error("[error] In TetMesh::loadV3d(): Unsupported Data Format " + format);
      }
    }
  }
}

void v3d::dx::SceneLoader::loadView()
{
  // setup namespace
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;

  // load view
  if (_jsonView.isNull()) throw std::runtime_error("[Error] empty json node");
  const auto& view = _jsonView;
  const auto method = view.get(METHOD_, "").toString();

  // load transfer functions
  loadTF();

  // load camera
  loadCamera();

  // create volume
  if (method == "REGULAR_GRID_VOLUME_RAY_CASTING")
  {
    // regular grid
    auto volume = std::make_shared<v3d::RegularGridVolumeGL>();
    {
      // configure volume
      auto data = std::dynamic_pointer_cast<RegularGridDataGL>(_data);
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
      if (_ctf) {
        volume->setTransferFunction(_ctf);
        volume->setTransferFunctionDirty(true);
        volume->setTransferFunctionDirtyCoarse(true);
      }
      if (_otf) {
        volume->setTransferFunction2D(_otf);
        volume->setTransferFunction2DDirty(true);
      }
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
      // load from json
      if (view.contains(VOLUME)) {
        const auto &jsonVol = view[VOLUME];
        fromJson(jsonVol, *volume);
      }
      // finalize
      _volume = volume;
    }

    // now we create a scene
    {
      // create the scene
      _sceneGrid = std::make_shared<RegularGridSceneGL>();
      _sceneGrid->setVolume(volume);
      _sceneGrid->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));
      _sceneGrid->setTFPreIntegration(false);
      _sceneGrid->setLighting(true);
      _sceneGrid->setEmptySpaceSkipping(false);
      _sceneGrid->setCamera(_camera);
      fromJson(view, *_sceneGrid);
    }

  }
  else if (method == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING")
  {

    PING;

  } else throw std::runtime_error("[Error] unknown volume type");
}

void v3d::dx::SceneLoader::loadTF()
{
  // setup namespace
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;
  // load TFN
  if (_jsonView.isNull()) throw std::runtime_error("[Error] empty json node");
  const auto& view = _jsonView;
  const auto method = view.get(METHOD_, "").toString();
  if (view.contains(VOLUME))
  {
    if (method == "REGULAR_GRID_VOLUME_RAY_CASTING") {
      auto volume = std::dynamic_pointer_cast<RegularGridVolumeGL>(_volume);
      const auto &jsonVol = view[VOLUME];
      if (jsonVol.contains(TRANSFER_FUNCTION)) {
        _ctf = std::make_shared<TransferFunction>(
            fromJson<TransferFunction>(jsonVol[TRANSFER_FUNCTION])
        );
        _ctf->updateColorMap();
      }
      if (jsonVol.contains(OCCLUSION_TRANSFER_FUNCTION)) {
        _otf = std::make_shared<OcclusionTransferFunction>(
            fromJson<OcclusionTransferFunction>(jsonVol[OCCLUSION_TRANSFER_FUNCTION])
        );
        _otf->update();
      }
    } else if (method == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING") {
      PING
    } else throw std::runtime_error("[Error] unknown volume type");
  }
}

void v3d::dx::SceneLoader::loadCamera()
{
  // setup namespace
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;
  // load camera
  if (_jsonView.isNull()) throw std::runtime_error("[Error] empty json node");
  const auto& view = _jsonView;
  const auto method = view.get(METHOD_, "").toString();
  if (view.contains(CAMERA)) {
    _camera = std::make_shared<Camera>(fromJson<Camera>(view[CAMERA]));
    _camera->setAspect(size.x / static_cast<float>(size.y));
  }
}

v3d::dx::SceneLoader::SceneLoader(std::string filename, int W, int H)
    : _jsonFileName(std::move(filename))
    , size{W, H}
{
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;
  JsonParser().load(_jsonFileName, _jsonRoot);
  if (_jsonRoot.contains(DATA_SOURCE)) {
    _jsonData = _jsonRoot[DATA_SOURCE];
  }
  if (_jsonRoot.contains(VIEW)) {
    _jsonView = _jsonRoot[VIEW];
  }
//  _ctf = std::move(TransferFunction::fromRainbowMap());
//  _otf = std::make_shared<OcclusionTransferFunction>();
}
