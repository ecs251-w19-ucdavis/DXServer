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
    auto loader = std::make_shared<RegularGridLoader>();
    loader->setFileName(common::PathRelToAbs(json.requires(FILE_NAME).toString(), std::move(jsonFileName)));
    loader->setOffset(size_t(json.requires(OFFSET).toInt64())); // maybe optional ?
    loader->setDimensions(fromJson<ivec3>(json.requires(DIMENSIONS)));
    loader->setType(typeFromJson(json.requires(TYPE)));
    loader->setEndian(endianFromJson(json.requires(ENDIAN)));
    loader->setFileUpperLeft(
        json.contains(FILE_UPPER_LEFT) ? json[FILE_UPPER_LEFT].toBool() : false
    );

    // load data
    auto medium = std::make_shared<RegularGridDataGL>();
    loader->setOutputData(medium.get());
    loader->update();
    medium->loadGL();

    // return
    return medium;
}
}}
