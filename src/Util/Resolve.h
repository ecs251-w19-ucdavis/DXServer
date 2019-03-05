//emove
// Created by Qi Wu on 2019-03-04.
//
#pragma once
#ifndef DXSERVER_RESOLVE_H
#define DXSERVER_RESOLVE_H

#include <functional>

namespace v3d { namespace dx {

namespace replies {

int add(const std::function<void()> &);

const std::function<void()> get(int);

const std::function<void()> pop(int);

}

}}

#endif //DXSERVER_RESOLVE_H
