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

using json_t   = JsonValue;
using rply_t   = std::function<void(JsonValue)>;
using reqt_t   = std::shared_ptr<Request>;
using queues_t = std::shared_ptr<RequestQueues>;

namespace queues {
void           create(); // we can only have one instance of RequestQueues
queues_t       get();
RequestQueues* raw();
}

///////////////////////////////////////////////////////////////////////////////

class Request {

public:
    Request(clid_t id, clid_t exp, int type, json_t request, rply_t resolve);
    int         getType()     const { return _type; }
    clid_t getClientId() const { return _id; }
    json_t      getRequest()  const { return _request; }
    rply_t  getResolve()  const { return _resolve; }
    bool isReady() const
    {
        return _exp == clients::get(_id)->currCounterValue(); // TODO is this implementation correct ?
    }
private:
    int _type;
    clid_t _id; // client id
    size_t      _exp; // expected counter value
    json_t      _request;
    rply_t  _resolve;
};

///////////////////////////////////////////////////////////////////////////////

class RequestQueues : public QObject {
    Q_OBJECT
    friend void queues::create();
public: 
    //void SetClientCounter();

    int dequeueCPU(clid_t &client_id, json_t& request, rply_t& resolve);
    int dequeueGPU(clid_t &client_id, json_t& request, rply_t& resolve);

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
    void EnqueueRequest(clid_t client_id, int type, json_t json, rply_t resolve);

private:
    RequestQueues() = default;
    void debugQueue(const std::deque<reqt_t>&);

private:
    int dequeue(std::deque<reqt_t>&, clid_t &client_id, json_t&, rply_t&);
    std::mutex _lock;
    std::deque<reqt_t> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
