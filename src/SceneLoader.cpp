//
// Created by Qi Wu on 2019-02-15.
//

#include "SceneLoader.h"


#include "Util/JsonParser.h"
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

using v3d::api::V3DGeometry;

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
  }
  if (json.contains(OFFSET)) {
    model_offset = json[OFFSET].toInt64();
  }
  if (json.contains(DIMENSIONS)) {
    model_dimensions = fromJson<ivec3>(json[DIMENSIONS]);
  }
  if (json.contains(TYPE)) {
    model_type = typeFromJson(json[TYPE]);
  }
  if (json.contains(ENDIAN)) {
    model_endian = endianFromJson(json[ENDIAN]);
  }
  if (json.contains(FILE_UPPER_LEFT)) {
    model_fileul = json[FILE_UPPER_LEFT].toBool();
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

V3DMedium v3d::dx::SceneLoader::load(std::string filename)
{
  // preparation
  using namespace v3d::serializer;
  using namespace v3d::dx::serializer;

  JsonValue root;
  JsonParser().load(filename, root);

  // load data source
  if (root.contains(DATA_SOURCE)) {
    const JsonValue& jsonArray = root[DATA_SOURCE];
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

//      v3d::api::V3DGeometry dataSrc;
//      if (format == FOLDER) {
//        dataSrc = new DataSourceFolder();
//      } else if (format == MULTIVARIATE) {
//        dataSrc = new MultivariateDataSource();
//      } else if (format == TIME_VARYING) {
//        dataSrc = new TimeVaryingDataSource();
//      } else if (format == REGULAR_GRID_RAW_BINARY) {
//        dataSrc = new RegularGridRawBinaryData();
//#ifdef V3D_USE_PVM
//      } else if (format == REGULAR_GRID_PVM) {
//        dataSrc = new RegularGridPVMData();
//#endif // V3D_USE_PVM
//      } else if (format == TETRAHEDRAL_GRID_RAW_BINARY) {
//        dataSrc = new TetraGridRawBinaryData();
//      } else if (format == TETRAHEDRAL_GRID_FAST) {
//        dataSrc = new TetraGridFASTData();
//      }

      if (format == REGULAR_GRID_RAW_BINARY) {
        return v3d::load::RegularGridRawBinary(jsonData, filename);
      }
      else {
        throw std::runtime_error("[error] In TetMesh::loadV3d(): Unsupported Data Format " + format);
      }
    }
  }
//  // load view
//  // -- we only load value range and transfer function for now
//  std::unique_ptr<V3DTfn> tfn(nullptr);
//  gdt::vec2f valuerange{ 1.f, -1.f };
//  if (root.contains(VIEW)) {
//    const JsonValue& view = root[VIEW];
//    std::string method = view.get(METHOD_, "").toString();
//    if (method == "REGULAR_GRID_VOLUME_RAY_CASTING") {
//      throw std::runtime_error("[error] In TetMesh::loadV3d(): We donot support structured grid");
//    }
//    else if (method == "TETRAHEDRAL_GRID_VOLUME_RAY_CASTING") {
//      if (view.contains(VOLUME)) {
//        const JsonValue& vol = view[VOLUME];
//        if (vol.contains(SCALAR_MAPPING_RANGE)) {
//          valuerange = rangeFromJson(vol[SCALAR_MAPPING_RANGE]);
//        }
//        if (vol.contains(TRANSFER_FUNCTION)) {
//          tfn = V3DTfn::fromJsonHeader(vol[TRANSFER_FUNCTION]);
//          tfn->updateColorMap();
//        }
//      }
//    }
//  }
//  if (tfn) {
//    tets->tfn = std::move(tfn);
//  }
//  if (valuerange.x < valuerange.y) {
//    tets->vertexAttributes[0]->valueRange.lower = valuerange.x;
//    tets->vertexAttributes[0]->valueRange.upper = valuerange.y;
//  }
//  return tets;

}