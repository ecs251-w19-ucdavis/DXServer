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

using json_t = v3d::JsonValue;
using response_t = std::function<void(v3d::JsonValue)>;
using request_t  = std::shared_ptr<Request>;
using queues_t   = std::shared_ptr<RequestQueues>;

namespace queues {
void create();
queues_t  get();
RequestQueues* raw();
}

///////////////////////////////////////////////////////////////////////////////

class Request {

public:
    Request(client_id_t id, client_id_t exp, int type, json_t request, response_t resolve);
    int              getType()     const { return _type; }
    client_id_t getClientId() const { return _id; }
    json_t      getRequest()  const { return _request; }
    response_t  getResolve()  const { return _resolve; }
    bool isReady() const
    {
        return _exp == clients::get(_id)->currCounterValue(); // TODO is this implementation correct ?
    }
private:
    int _type;
    client_id_t _id; // client id
    client_id_t _exp; // expected counter value
    json_t      _request;
    response_t  _resolve;
};

///////////////////////////////////////////////////////////////////////////////

class RequestQueues : public QObject {
    Q_OBJECT
public: 
    //void SetClientCounter();

    int dequeueCPU(client_id_t &client_id, json_t& request, response_t& resolve);
    int dequeueGPU(client_id_t &client_id, json_t& request, response_t& resolve);

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
    void EnqueueRequest(client_id_t client_id, int type, json_t json, response_t resolve);

private:
    int dequeue(std::deque<request_t>&, client_id_t &client_id, json_t&, response_t&);
    std::mutex _lock;
    std::deque<request_t> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
