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

#include "Util/JsonParser.h"
#include "Util/Client.h"
#include "Util/Request.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QObject>

#include <functional>
#include <deque>
#include <string>
#include <mutex>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

class RequestQueues;
using queues_t = std::shared_ptr<RequestQueues>;

namespace queues {
void           create(); // we can only have one instance of RequestQueues
queues_t       get();
RequestQueues* raw();
}

///////////////////////////////////////////////////////////////////////////////

class RequestQueues : public QObject {
    Q_OBJECT
    friend void queues::create();
public: 
    //void SetClientCounter();
    void enqueue(clid_t client_id, clid_t request_id, int type, json_t json, rply_t resolve);
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
    void enqueueRequest(clid_t client_id, int type, json_t json, rply_t resolve);

private:
    RequestQueues() = default;
    void debugQueue(const std::deque<rqst_t>&);

private:
    int dequeue(std::deque<rqst_t>&, clid_t &client_id, json_t&, rply_t&);
    std::mutex _lock;
    std::deque<rqst_t> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
