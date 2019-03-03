//
// Created by Qi Wu on 2019-02-23.
//

#pragma once
#ifndef DXSERVER_REQUESTHANDLER_H
#define DXSERVER_REQUESTHANDLER_H

#include "RequestQueue.h"
#include "Util/JsonParser.h"

namespace v3d { namespace dx {

/**
 * This class suppose to handle requests from the event queue. This class will open and run in a different thread.
 */
class CPUTaskHandler {
public:
    explicit CPUTaskHandler(const std::string& database = "database.json");
private:
    void loadDatabase(const std::string& database);

    void handleQueryDatabase(int clientId, v3d::JsonValue& output);
    void handleOpenProjectRequested(std::string projFileName, int clientId);
    void handleCloseProjectRequested(int clientId);
    void handleGetSceneRequested(int64_t id, int clientId);
    void handleFrameRequested(const v3d::JsonValue& scene, int clientId);

private:
    v3d::JsonValue _jsonDatabase;

    /** TODO This is just an experimental implementation, we should change it to a client list instead. The length
     *       of the client list should be identical to the length in WebSocketCommunicator
     */
//    std::shared_ptr<Engine> engine;
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
