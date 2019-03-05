//
// Created by Qi Wu on 2019-03-05.
//

#pragma once
#ifndef DXSERVER_REQUEST_H
#define DXSERVER_REQUEST_H

#include "Util/JsonParser.h"
#include "Util/Client.h"

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

class Request;
using json_t = JsonValue;
using rply_t = std::function<void(JsonValue)>;
using rqst_t = std::shared_ptr<Request>;

///////////////////////////////////////////////////////////////////////////////

class Request {
public:
    Request(clid_t id, clid_t exp, int type, json_t request, rply_t resolve);
    int    getType()     const { return _type; }
    clid_t getClientId() const { return _id; }
    json_t getRequest()  const { return _request; }
    rply_t getResolve()  const { return _resolve; }
    bool isReady() const
    {
        return _exp == clients::get(_id)->currCounterValue(); // TODO is this implementation correct ?
    }
private:
    int    _type;
    clid_t _id; // client id
    size_t _exp; // expected counter value
    json_t _request;
    rply_t _resolve;
};

}}

#endif //DXSERVER_REQUEST_H
