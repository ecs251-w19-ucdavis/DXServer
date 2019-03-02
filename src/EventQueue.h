//
// Created by Qi Wu on 2019-02-23.
//
#pragma once
#ifndef DXSERVER_EVENTQUEUE_H
#define DXSERVER_EVENTQUEUE_H

#include "Util/JsonParser.h"

#include <QWebSocket>
#include <QWebSocketServer>

#include <functional>
#include <vector>
#include <string>

#include <QObject>

namespace v3d {
namespace dx {

class Event {
    using response_t = std::function<void(v3d::JsonValue)>;
public:
    explicit Event(int ClientId, int type, v3d::JsonValue request, response_t resolve);
    int GetClient()
    { return _ClientId; }
    int GetType()
    { return _type; }

private:
    int _ClientId;
    int _type;
    v3d::JsonValue _request;
    response_t _resolve;
};

class EventQueue: public QObject {
Q_OBJECT
    using response_t = std::function<void(v3d::JsonValue)>;
public:
    std::vector<Event>::iterator FindClientType(int clientId, int type);
public slots:
    void AddNewRequest(int clientId, int type, v3d::JsonValue request, response_t resolve);
signals:
    void handleRequestFrame(int clientId, int type, v3d::JsonValue request, response_t resolve);
    void handleOpenProject(int clientId, int type, v3d::JsonValue request, response_t resolve);
    void handleCloseProject(int clientId, int type, v3d::JsonValue request, response_t resolve);
    void handleGetScene(int clientId, int type, v3d::JsonValue request, response_t resolve);
    void handleQueryDatabase(int clientId, int type, v3d::JsonValue request, response_t resolve);
private:
    std::vector<Event> Events;
};

}
}

#endif //DXSERVER_EVENTQUEUE_H
