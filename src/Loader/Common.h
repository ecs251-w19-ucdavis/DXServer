//===========================================================================//
//                                                                           //
// LibViDi3D                                                                 //
// Copyright(c) 2018 Qi Wu (Wilson)                                          //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_COMMON_H
#define DXSERVER_COMMON_H

#include <string>

namespace v3d { namespace common {

inline std::string FixSlash(std::string path)
{
#ifdef WIN32
    std::replace(path.begin(), path.end(), '/', '\\');
#else
    std::replace(path.begin(), path.end(), '\\', '/');
#endif
    return path;
}

inline std::string PathRelToAbs(std::string filename, std::string headername)
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

}}

#endif //DXSERVER_COMMON_H
