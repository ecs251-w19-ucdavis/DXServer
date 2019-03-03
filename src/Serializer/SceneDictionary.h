//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_SCENEDICTIONARY_H
#define DXSERVER_SCENEDICTIONARY_H

namespace v3d {
namespace dx {
namespace serializer {

// from JsonSerializer.h
const char* const CAMERA = "camera";
const char* const CAPTION = "caption";
const char* const CELL_COUNT = "cellCount";
const char* const CELLS_OFFSET = "cellsOffset";
const char* const DATA_FILE_NAME = "dataFileName";
const char* const DATA_ID = "dataId";
const char* const DATA_SOURCE = "dataSource";
const char* const DIMENSIONS = "dimensions";
const char* const ENDIAN = "endian";
const char* const FILE_NAME = "fileName";
const char* const FILE_UPPER_LEFT = "fileUpperLeft";
const char* const FORMAT = "format";
const char* const GRID_FILE_NAME = "gridFileName";
const char* const ID = "id";
const char* const IMAGE = "image";
const char* const METHOD_ = "method";
const char* const NAME = "name";
const char* const OFFSET = "offset";
const char* const POINT_COUNT = "pointCount";
const char* const POINT_DATA_OFFSET = "pointDataOffset";
const char* const POINTS_OFFSET = "pointsOffset";
const char* const SECONDARY_DATA_ID = "secondaryDataId";
const char* const SNAPSHOT = "snapshot";
const char* const SOLUTION_FILE_NAME = "solutionFileName";
const char* const TRANSFER_FUNCTION = "transferFunction";
const char* const OCCLUSION_TRANSFER_FUNCTION = "occlusionTransferFunction";
const char* const VARIABLE_TYPE = "variableType";
const char* const VIEW = "view";
const char* const VOLUME = "volume";

// from DataSourceModel.h
const char *const FOLDER = "FOLDER";
const char *const MULTIVARIATE = "MULTIVARIATE";
#ifdef V3D_USE_PVM
const char *const REGULAR_GRID_PVM = "REGULAR_GRID_PVM";
#endif
const char *const REGULAR_GRID_RAW_BINARY = "REGULAR_GRID_RAW_BINARY";
const char *const TETRAHEDRAL_GRID_FAST = "TETRAHEDRAL_GRID_FAST";
const char *const TETRAHEDRAL_GRID_RAW_BINARY = "TETRAHEDRAL_GRID_RAW_BINARY";
const char *const TIME_VARYING = "TIME_VARYING";

} // namespace serializer
} // namespace dx
} // namespace v3d

#endif //DXSERVER_SCENEDICTIONARY_H
