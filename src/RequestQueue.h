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
queues_t       get();
RequestQueues* raw();
}

///////////////////////////////////////////////////////////////////////////////

namespace details {
queues_t createRequestQueues(); // we can only have one instance of RequestQueues
}

class RequestQueues : public QObject {
    Q_OBJECT
    friend queues_t details::createRequestQueues();
public: 

    // TODO the name of this function is misleading
    void enqueue(clid_t client_id, size_t request_id, int type, json_t json, rply_t resolve);

    /**
     * Dequeue from the CPU queue
     * @param client_id
     * @param request
     * @param resolve
     * @return 1 means successful, 0 means error
     */
    int dequeueCPU(clid_t &client_id, json_t& request, rply_t& resolve);

    /**
     * Dequeue from the GPU queue
     * @param client_id
     * @param request
     * @param resolve
     * @return 1 means successful, 0 means error
     */
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
    /**
     * Constructor
     * @note This constructor is protected because there can be only one instance of this class.
     */
    RequestQueues() = default;

    //! debugger
    void debugQueue(const std::deque<rqst_t>&);

private:
    int dequeue(std::deque<rqst_t>&, clid_t &client_id, json_t&, rply_t&);
    std::mutex _lock;
    std::deque<rqst_t> _queue_cpu, _queue_gpu;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
