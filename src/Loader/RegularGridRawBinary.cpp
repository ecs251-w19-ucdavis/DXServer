//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "RegularGridRawBinary.h"

#include "Serializer/Dictionary.h"
#include "Serializer/SceneDictionary.h"
#include "Serializer/VectorSerializer.h"
#include "Serializer/VolumeSerializer.h"
#include "Scene/IGeometry.h"
#include "Scene/IMedium.h"
#include "Scene/Medium/RegularGridDataGL.h"

#include "Data/RegularGridLoader.h"

#include "Loader/Common.h"

using v3d::JsonValue;
using v3d::JsonParser;

namespace v3d { namespace load {
api::V3DMedium RegularGridRawBinary(const JsonValue& json, std::string jsonFileName)
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

    // get filename
    std::string model_filename;
    int64_t     model_offset;
    ivec3       model_dimensions;
    Type        model_type;
    Endian      model_endian;
    bool        model_fileul;
    if (json.contains(FILE_NAME)) {
        model_filename = common::PathRelToAbs(json[FILE_NAME].toString(), jsonFileName);
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
