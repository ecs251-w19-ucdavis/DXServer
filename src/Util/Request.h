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

namespace details { class Request; }
using rply_t = std::function<void(JsonValue)>;
using rqst_t = std::shared_ptr<details::Request>;

/**
 * Through this namespace, we can create a new request and retrieve its shared pointer
 */
namespace requests {

/**
 * This is a helper function to create a new request
 * @param id
 * @param exp
 * @param type
 * @param request
 * @param resolve
 * @return
 */
rqst_t create(const clid_t &id,
              const size_t &exp,
              const int &type,
              const json_t &request,
              const rply_t &resolve);
};

///////////////////////////////////////////////////////////////////////////////

namespace details {

class Request {
    friend rqst_t requests::create(const clid_t &id,
                                   const size_t &exp,
                                   const int &type,
                                   const json_t &request,
                                   const rply_t &resolve);
public:
    int    getType()     const { return _type; }
    clid_t getClientId() const { return _id; }
    json_t getRequest()  const { return _request; }
    rply_t getResolve()  const { return _resolve; }
    bool   isReady()     const { return _exp == (clients::get(_id)->currCounterValue()); }

private:
    /**
     * Constructor
     * @param id
     * @param exp
     * @param type
     * @param request
     * @param resolve
     */
    Request(clid_t id, size_t exp, int type, json_t request, rply_t resolve);

private:
    int    _type;
    clid_t _id; // client id
    size_t _exp; // expected counter value
    json_t _request;
    rply_t _resolve;
};

}

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_REQUEST_H
