//
// Created by Qi Wu on 2019-02-15.
//

#pragma once
#ifndef DXSERVER_SCENELOADER_H
#define DXSERVER_SCENELOADER_H

#include "Scene/IGeometry.h"
#include "Scene/IMedium.h"

#include <string>

using v3d::api::V3DMedium;

namespace v3d { namespace dx {

class SceneLoader {
public:
    SceneLoader() = default;
    V3DMedium load(std::string filename);

protected:

};

}
}

#endif //DXSERVER_SCENELOADER_H
