//
// Created by Qi Wu on 2019-03-04.
//

#pragma once
#ifndef DXSERVER_RESOLVE_H
#define DXSERVER_RESOLVE_H

#include <functional>

namespace v3d { namespace dx {

/** A resolve means the function that use to reply a request. The whole process works like the following:
 *  TODO DOC
 */
namespace resolves {

/**
 * TODO DOC
 * @return
 */
int add(const std::function<void()>&);

/**
 * TODO DOC
 * @return
 */
const std::function<void()> pop(int);

}

}}

#endif //DXSERVER_RESOLVE_H
