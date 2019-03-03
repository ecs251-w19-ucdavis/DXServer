//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef DXSERVER_REQUESTQUEUE_H
#define DXSERVER_REQUESTQUEUE_H

#include "Client.h"
#include "Util/JsonParser.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QObject>

#include <functional>
#include <deque>
#include <string>
#include <mutex>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

class Request;
class RequestQueues;

namespace api {
using json_t = v3d::JsonValue;
using response_t = std::function<void(v3d::JsonValue)>;
using request_t  = std::shared_ptr<Request>;
using queues_t   = std::shared_ptr<RequestQueues>;
};

namespace queues {
void create();
api::queues_t  get();
RequestQueues* raw();
}

///////////////////////////////////////////////////////////////////////////////

class Request {

public:
    Request(api::client_id_t id, api::client_id_t exp, int type, api::json_t request, api::response_t resolve);
    int              getType()     const { return _type; }
    api::client_id_t getClientId() const { return _id; }
    api::json_t      getRequest()  const { return _request; }
    api::response_t  getResolve()  const { return _resolve; }
    bool isReady() const
    {
        return _exp == clients::get(_id)->currCounterValue(); // TODO is this implementation correct ?
    }
private:
    int _type;
    api::client_id_t _id; // client id
    api::client_id_t _exp; // expected counter value
    api::json_t      _request;
    api::response_t  _resolve;
};

///////////////////////////////////////////////////////////////////////////////

class RequestQueues : public QObject {
    Q_OBJECT
public: 
    //void SetClientCounter();

    int dequeueCPU(api::client_id_t &client_id, api::json_t& request, api::response_t& resolve);
    int dequeueGPU(api::client_id_t &client_id, api::json_t& request, api::response_t& resolve);

public slots: // <- NOTE don't forget this slots keyword defined by Qt
    // This is an example for implementing a QObject slot
    // For more information, check https://doc.qt.io/qt-5/signalsandslots.html
    /**
     * To add a new request into the request queue. Called from v3d::dx::WebSocketCommunicator.
     * @param client_id
     * @param type
     * @param request
     * @param resolve
     */
    void EnqueueRequest(api::client_id_t client_id, int type, api::json_t json, api::response_t resolve);

private:
    int dequeue(std::deque<api::request_t>&, api::client_id_t &client_id, api::json_t&, api::response_t&);
    std::mutex _lock;
    std::deque<api::request_t> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
