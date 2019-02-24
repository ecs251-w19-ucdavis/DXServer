//
// Created by Qi Wu on 2019-02-23.
//

#pragma once
#ifndef DXSERVER_REQUESTHANDLER_H
#define DXSERVER_REQUESTHANDLER_H

#include "Util/JsonParser.h"

#include <QObject>

namespace v3d { namespace dx {

/**
 * This class suppose to handle requests from the event queue. This class will open and run in a different thread.
 */
class RequestHandler : public QObject {
    Q_OBJECT
    using response_t = std::function<void(v3d::JsonValue)>;
public:
    RequestHandler(const std::string& database = "database.json");
    void handleQueryDatabase(int clientId, v3d::JsonValue& output);
//    void handle_OpenProjectRequested(std::string projFileName, int clientId);
//    void handle_CloseProjectRequested(int clientId);
//    void handle_GetSceneRequested(int64_t id, int clientId);
//    void handle_FrameRequested(v3d::JsonValue scene, int clientId);

public slots: // <- NOTE don't forget this slots keyword defined by Qt
    // This is an example for implementing a QObject slot
    // For more information, check https://doc.qt.io/qt-5/signalsandslots.html
    void handleNewRequest(int clientId, int type, v3d::JsonValue request, response_t resolve);

private:
    v3d::JsonValue _jsonDatabase;
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
