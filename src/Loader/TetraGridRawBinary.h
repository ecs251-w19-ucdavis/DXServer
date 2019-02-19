//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_TETRAGRIDRAWBINARY_H
#define DXSERVER_TETRAGRIDRAWBINARY_H

#include "Util/JsonParser.h"
#include "Scene/IMedium.h"

namespace v3d { namespace load {
api::V3DMedium TetraGridRawBinaryData(const JsonValue &json, std::string jsonFileName);
}}

#endif //DXSERVER_TETRAGRIDRAWBINARY_H
