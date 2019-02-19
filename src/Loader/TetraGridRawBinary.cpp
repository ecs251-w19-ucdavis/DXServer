//
// Created by Qi Wu on 2019-02-18.
//

#include "TetraGridRawBinary.h"

#include "Serializer/Dictionary.h"
#include "Serializer/SceneDictionary.h"
#include "Serializer/VectorSerializer.h"
#include "Serializer/VolumeSerializer.h"
#include "Scene/IGeometry.h"
#include "Scene/IMedium.h"
#include "Scene/Medium/TetraGridDataGL.h"

#include "Data/TetraGridLoader.h"

#include "Loader/Common.h"

using v3d::JsonValue;
using v3d::JsonParser;

namespace v3d { namespace load {
api::V3DMedium TetraGridRawBinaryData(const JsonValue &json, std::string jsonFileName)
{
    // output
    api::V3DGeometry ret;

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

    // get parameters
    auto loader = std::make_shared<TetraGridLoader>();
    loader->setGridFileName(common::PathRelToAbs(json.requires(GRID_FILE_NAME).toString(), std::move(jsonFileName)));
    loader->setDataFileName(common::PathRelToAbs(json.requires(DATA_FILE_NAME).toString(), std::move(jsonFileName)));
    loader->setPointCount(json.requires(POINT_COUNT).toInt());
    loader->setCellCount(json.requires(CELL_COUNT).toInt());
    loader->setPointsOffset(size_t(json.requires(POINTS_OFFSET).toInt64()));
    loader->setCellsOffset(size_t(json.requires(CELLS_OFFSET).toInt64()));
    loader->setPointDataOffset(size_t(json.requires(POINT_DATA_OFFSET).toInt64()));

    // load
    auto medium = std::make_shared<TetraGridDataGL>();
    loader->setOutputData(medium.get());
    loader->update();
    medium->grid()->correctVertexOrder();
    medium->grid()->buildCellToCellConnectivity();
    medium->grid()->buildBoundaryMesh();
    medium->computePointGradient();
    std::dynamic_pointer_cast<TetraGridGL>(medium)->loadGL();
    medium->loadGL();

    // return
    return medium;
}
}}
