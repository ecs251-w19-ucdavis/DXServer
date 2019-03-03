//
// Created by Qi Wu on 2019-02-23.
//

#include "CPUTaskHandler.h"

#include "Util/Log.h"

#include <QBuffer>
#include <QImage>

namespace v3d { namespace dx {

CPUTaskHandler::CPUTaskHandler(const std::string &database)
{
    loadDatabase(database);
}

void CPUTaskHandler::loadDatabase(const std::string& database)
{
    // open the database file as a string
    std::ifstream dsFile(database);
    if (!dsFile.is_open()) {
        // I want to avoid crashing simply because database.json does not exists.
        log() << "[error] cannot open database lookup file "
              << "'"
              << database
              << "' please generate one by re-run cmake"
              << std::endl;
        return;
    }
    std::string dsString((std::istreambuf_iterator<char>(dsFile)), std::istreambuf_iterator<char>());

    // convert the string into JSON
    try { _jsonDatabase = JsonParser().parse(dsString); }
    catch (std::exception &) {
        throw std::runtime_error("[Error] invalid database lookup file " + database);
    }

    // convert preview image into base64 string
    if (_jsonDatabase.isArray()) {
        JsonValue::Array &dsArray = _jsonDatabase.toArray();
        for (auto &ds : dsArray) {
            QImage img;
            if (img.load(ds["preview"].toString().c_str())) {
                QByteArray ba;
                QBuffer buf(&ba);
                buf.open(QIODevice::WriteOnly);
                img.save(&buf, "JPG");
                buf.close();
                QByteArray base64 = ba.toBase64();
                ds["preview"] = "data:image/jpeg;base64," + base64.toStdString();
            }
            else {
                throw std::runtime_error("[Error] failed to load data preview image " + ds["preview"].toString());
            }
        }
    }
    else {
        throw std::runtime_error("[Error] invalid database lookup file " + database);
    }
}

void CPUTaskHandler::handleQueryDatabase(int clientId, v3d::JsonValue &output)
{
    if (!_jsonDatabase.isNull()) { output = _jsonDatabase; } // make a copy
}

void CPUTaskHandler::handleOpenProjectRequested(std::string projFileName, int clientId)
{

}

void CPUTaskHandler::handleCloseProjectRequested(int clientId)
{

}

void CPUTaskHandler::handleGetSceneRequested(int64_t id, int clientId)
{

}

void CPUTaskHandler::handleFrameRequested(const v3d::JsonValue &scene, int clientId)
{

}

//
//void v3d::dx::RequestHandler::handleNewRequest(int clientId, int type, v3d::JsonValue json,
//                                               v3d::dx::RequestHandler::response_t resolve)
//{
//    std::string method = json.get("method", "").toString();
//
//    if (method == "queryDatabase") {
//
//        v3d::JsonValue output;
//        handleQueryDatabase(clientId, output);
//        resolve(output);
//
//    } else if (method == "openProject") {
//
//        std::string filename;
//        if (json.contains("params") &&
//            json["params"].isObject() &&
//            json["params"].contains("fileName") &&
//            json["params"]["fileName"].isString())
//        {
//            filename = json["params"]["fileName"].toString();
//        }
//        if (!filename.empty())
//        {
//            // enable this once the implementation has been done
//            // no input required, so passing a dummy v3d::JsonValue instead
////            resolve(v3d::JsonValue());
//        }
//
//    } else if (method == "closeProject") {
//
//        // enable this once the implementation has been done
//        // no input required, so passing a dummy v3d::JsonValue instead
////        resolve(v3d::JsonValue());
//
//    } else if (method == "getScene") {
//
////        int64_t id = json.get("id", -1).toInt64();
////        emit getSceneRequested(id, clientId);
//
//    } else if (method == "requestFrame") {
//
////        // sample message:
////        //  {
////        //    "jsonrpc": "2.0",
////        //    "method": "requestFrame",
////        //    "params": {
////        //      "scene": {...}
////        //    }
////        //  }
////        JsonValue scene;
////        if (json.contains("params") && json["params"].isObject() && json["params"].contains("scene")) {
////            scene = json["params"]["scene"];
////        }
////        emit frameRequested(scene, clientId);
//
//    }
//}

}}