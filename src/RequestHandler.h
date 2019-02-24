//
// Created by Qi Wu on 2019-02-23.
//

#pragma once
#ifndef DXSERVER_REQUESTHANDLER_H
#define DXSERVER_REQUESTHANDLER_H

#include "SceneHandler.h"

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
    explicit RequestHandler(const std::string& database = "database.json");
    void handleQueryDatabase(int clientId, v3d::JsonValue& output);
    void handleOpenProjectRequested(std::string projFileName, int clientId);
    void handleCloseProjectRequested(int clientId);
    void handleGetSceneRequested(int64_t id, int clientId);
    void handleFrameRequested(const v3d::JsonValue& scene, int clientId);

public slots: // <- NOTE don't forget this slots keyword defined by Qt
    // This is an example for implementing a QObject slot
    // For more information, check https://doc.qt.io/qt-5/signalsandslots.html
    void handleNewRequest(int clientId, int type, v3d::JsonValue request, response_t resolve);

private:
    v3d::JsonValue _jsonDatabase;

    /** TODO This is just an experimental implementation, we should change it to a client list instead. The length
     *       of the client list should be identical to the length in WebSocketCommunicator
     */
//    std::shared_ptr<SceneHandler> engine;
};

}}


#endif //DXSERVER_REQUESTHANDLER_H