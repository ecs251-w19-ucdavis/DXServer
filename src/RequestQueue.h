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

#include <QWebSocket>
#include <QWebSocketServer>
#include <QObject>

#include <functional>
#include <deque>
#include <string>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace api {
using response_t = std::function<void(v3d::JsonValue)>;
};

///////////////////////////////////////////////////////////////////////////////

class Request {

public:
    Request(int client_id, int type, v3d::JsonValue request, api::response_t resolve);
    int     getRequestType() const { return _type; }
    int64_t getClientId()    const { return _client_id; }
    //bool IsValid() {return _expectation == RequestCounters[_client_id]};
private:
    int _type;
    int64_t _expectation; // expected counter value
    int64_t _client_id;
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
private: 
    std::deque<Request> QueueCPU, QueueGPU;
};

///////////////////////////////////////////////////////////////////////////////

}}

#endif //DXSERVER_EVENTQUEUE_H
