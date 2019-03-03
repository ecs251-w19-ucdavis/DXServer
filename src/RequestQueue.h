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

#include <functional>
#include <deque>
#include <string>

#include <QObject>

namespace v3d {
namespace dx {

class Request {
    using response_t = std::function<void(v3d::JsonValue)>;
public:
    Request(int client_id, int type, v3d::JsonValue request, response_t resolve);
    int     getRequestType() const { return _type; }
    int64_t getClientId()    const { return _client_id; }
    //bool IsValid() {return _expectation == RequestCounters[_client_id]};
private:
    int _type;
    int64_t _expectation; // expected counter value
    int64_t _client_id;
    JsonValue _request;
    response_t _resolve;
};

class RequestQueues : public QObject {
    Q_OBJECT
    using response_t = std::function<void(v3d::JsonValue)>;
public: 
    void SetClientCounter();
public slots:
    void EnqueueRequest(int client_id, int type, v3d::JsonValue request, response_t resolve);
private: 
    std::deque<Request> QueueCPU, QueueGPU;
};

}
}

#endif //DXSERVER_EVENTQUEUE_H
