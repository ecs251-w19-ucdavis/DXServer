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

//class RequestQueue;
//
//using queues_t = std::shared_ptr<RequestQueue>;

//namespace queues {
//queues_t       get();
//RequestQueue* raw();
//}

///////////////////////////////////////////////////////////////////////////////

//namespace details {
//queues_t createRequestQueues(); // we can only have one instance of RequestQueue
//}

class RequestQueue : public QObject {
    Q_OBJECT
//    friend queues_t details::createRequestQueues();
public:
    /**
     * Constructor
     * @note This constructor is protected because there can be only one instance of this class.
     */
    RequestQueue() = default;
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
    void newRequest(clid_t client_id, int type, json_t json, rply_t resolve);

private:


    //! debugger
    void debugQueue(const std::deque<rqst_t>&);

private:
    int  dequeue(std::deque<rqst_t>&, clid_t &client_id, json_t&, rply_t&);
    void enqueue(std::deque<rqst_t>&, const clid_t &client_id, size_t request_id,
                 int type, const json_t &json, const rply_t &resolve);
    std::mutex _lock;
    std::deque<rqst_t> _central_queue, _graphic_queue; // I renamed them to avoid mis-reading
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
