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

class TaskHandler;

///////////////////////////////////////////////////////////////////////////////

class RequestQueue : public QObject {
    Q_OBJECT
public:
    /**
     * Constructor
     * @note This constructor is protected because there can be only one instance of this class.
     */
    RequestQueue() = default;

    /**
     *
     * @param handler
     */
    void connectToHandlerCPU(std::shared_ptr<TaskHandler> handler) { _central_hanlder = std::move(handler); }

    /**
     *
     * @param handler
     */
    void connectToHandlerGPU(std::shared_ptr<TaskHandler> handler) { _graphic_hanlder = std::move(handler); }

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
    /**
     * Helper function for dequeue
     * @param queue
     * @param client_id
     * @return
     */
    int  dequeue(std::deque<rqst_t>& queue, clid_t &client_id, json_t&, rply_t&);
    /**
     * Helper function for enqueue
     * @param queue
     * @param client_id
     * @param request_id
     * @param type
     * @param json
     * @param resolve
     */
    void enqueue(std::deque<rqst_t>& queue,
                 const clid_t &client_id,
                 const size_t &request_id,
                 const int    &type,
                 const json_t &json,
                 const rply_t &resolve);
private:
    std::mutex         _queue_lock;
    std::deque<rqst_t> _central_queue;
    std::deque<rqst_t> _graphic_queue; // I renamed them to avoid mis-reading
    std::shared_ptr<TaskHandler> _central_hanlder;
    std::shared_ptr<TaskHandler> _graphic_hanlder;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
