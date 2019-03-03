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
    Request(api::client_id_t client_id, int type, v3d::JsonValue request, api::response_t resolve);
    int              getType()     const { return _type; }
    api::client_id_t getClientId() const { return _client_id; }
    JsonValue        getRequest()  const { return _request; }
    api::response_t  getResolve()  const { return _resolve; }
    bool isReady() const
    {
        PING; // TODO we need to finish one implementation here
        return _expectation == -1;
    }
private:
    int _type;
    api::client_id_t _expectation; // expected counter value
    api::client_id_t _client_id;
    JsonValue _request;
    api::response_t _resolve;
};

///////////////////////////////////////////////////////////////////////////////

class RequestQueues : public QObject {
    Q_OBJECT
public: 
    void SetClientCounter();
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
    void EnqueueRequest(int client_id, int type, v3d::JsonValue request, api::response_t resolve);

    int dequeueCPU(api::client_id_t &client_id, v3d::JsonValue& request, api::response_t& resolve);
    int dequeueGPU(api::client_id_t &client_id, v3d::JsonValue& request, api::response_t& resolve);
private:
    int dequeue(std::deque<Request>&, api::client_id_t &client_id, v3d::JsonValue&, api::response_t&);

    std::mutex _lock;
    std::deque<Request> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
