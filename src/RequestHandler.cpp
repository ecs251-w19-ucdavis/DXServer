//
// Created by Qi Wu on 2019-02-23.
//

#include "RequestHandler.h"

#include "Util/Log.h"

#include <QBuffer>
#include <QImage>

v3d::dx::RequestHandler::RequestHandler(const std::string& database)
    : QObject()
{
    std::ifstream dsFile(database);
    if (!dsFile.is_open()) {
        log() << "[error] cannot open database meta-information file "
              << "'"
              << database
              << "' please generate one."
              << std::endl;
        return;
    }
    std::string dsString((std::istreambuf_iterator<char>(dsFile)), std::istreambuf_iterator<char>());
    try { _jsonDatabase = JsonParser().parse(dsString); }
    catch (std::exception &) {
        log() << "[error] Invalid JSON file '" << database << "'" << std::endl;
        return;
    }
    log() << "[Database] finished reading database meta-info" << std::endl;
}

void v3d::dx::RequestHandler::handleQueryDatabase(int clientId, v3d::JsonValue& output)
{
    if (!_jsonDatabase.isNull()) { output = _jsonDatabase; }
}

void v3d::dx::RequestHandler::handleNewRequest(int clientId, int type, v3d::JsonValue json,
                                               v3d::dx::RequestHandler::response_t resolve)
{
    std::string method = json.get("method", "").toString();
    log() << "[debug] message received " << method << std::endl;

    if (method == "queryDatabase") {
        v3d::JsonValue output;
        handleQueryDatabase(clientId, output);
        resolve(output);
    }
//    else if (method == "openProject") {
//
//        std::string projFileName;
//        if (json.contains("params") &&
//            json["params"].isObject() &&
//            json["params"].contains("fileName") &&
//            json["params"]["fileName"].isString()) {
//            projFileName = json["params"]["fileName"].toString();
//        }
//        if (!projFileName.empty()) {
//            emit openProjectRequested(projFileName, clientId);
//        }
//
//    }
//    else if (method == "closeProject") {
//
//        emit closeProjectRequested(clientId);
//
//    }
//    else if (method == "getScene") {
//
//        int64_t id = json.get("id", -1).toInt64();
//        emit getSceneRequested(id, clientId);
//
//    }
//    else if (method == "requestFrame") {
//
//        // sample message:
//        //  {
//        //    "jsonrpc": "2.0",
//        //    "method": "requestFrame",
//        //    "params": {
//        //      "scene": {...}
//        //    }
//        //  }
//        JsonValue scene;
//        if (json.contains("params") && json["params"].isObject() && json["params"].contains("scene")) {
//            scene = json["params"]["scene"];
//        }
//        emit frameRequested(scene, clientId);
//
//    }
}