//
// Created by Qi Wu on 2019-03-05.
//

#include "Request.h"

using namespace v3d::dx;

Request::Request(clid_t id, clid_t exp, int type, json_t request, rply_t resolve)
    : _id(id)
    , _exp(exp)
    , _type(type)
    , _request(std::move(request))
    , _resolve(std::move(resolve))
{}
