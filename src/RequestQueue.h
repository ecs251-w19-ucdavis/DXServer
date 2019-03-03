//
// Created by Qi Wu on 2019-02-23.
//
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

namespace v3d { namespace dx {
class Request {
    using response_t = std::function<void(v3d::JsonValue)>;
public:
    explicit Request(int ClientId, int type, v3d::JsonValue request, response_t resolve);
    int GetClient() {return _ClientId;}
    int GetType() {return _type;}
    //bool IsValid() {return _Expectation == RequestCounters[_ClientId]};
private: 
    int _Expectation; // expected counter value
    int _ClientId;
    int _type;
    v3d::JsonValue _request;
    response_t _resolve;
};

class RequestQueues : public QObject {
    Q_OBJECT
    using response_t = std::function<void(v3d::JsonValue)>;
public: 
    void SetClientCounter();
public slots:
    void EnqueueRequest(int clientId, int type, v3d::JsonValue request, response_t resolve);
private: 
    std::deque<Request> QueueCPU, QueueGPU;
};

}}

#endif //DXSERVER_EVENTQUEUE_H
