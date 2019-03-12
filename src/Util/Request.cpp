//
// Created by Qi Wu on 2019-03-05.
//

#include "Request.h"

using namespace v3d::dx;

///////////////////////////////////////////////////////////////////////////////

rqst_t requests::create(const clid_t &id,
                        const size_t &exp,
                        const int    &type,
                        const json_t &request,
                        const rply_t &resolve)
{
    std::shared_ptr<details::Request> ret;
    ret.reset(new details::Request(id, exp, type, request, resolve));
    return std::move(ret);
}

///////////////////////////////////////////////////////////////////////////////

details::Request::Request(clid_t id, size_t exp, int type, json_t request, rply_t resolve)
    : _id(id)
    , _exp(exp)
    , _type(type)
    , _request(std::move(request))
    , _resolve(std::move(resolve))
{}

///////////////////////////////////////////////////////////////////////////////