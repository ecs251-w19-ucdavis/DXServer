#include "WebSocketCommunicator.h"

#include "RequestHandler.h"
#include "EventQueue.h"

#include "Util/Log.h"

#include <QBuffer>

using namespace v3d;

v3d::dx::WebSocketCommunicator::WebSocketCommunicator(quint16 port, QObject *parent)
    : QObject(parent), _port(port)
{
}

void v3d::dx::WebSocketCommunicator::open()
{
    auto sslMode = _secureMode ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode;
    _webSocketServer = new QWebSocketServer(QStringLiteral("Vidi3D Render Server"), sslMode, this);
    if (_webSocketServer->listen(QHostAddress::Any, _port)) {
        log() << ("[Server] Render server listening on port " + QString("%1").arg(_port, 0, 10)).toStdString()
              << std::endl;
        connect(_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketCommunicator::onNewConnection);
        connect(_webSocketServer, &QWebSocketServer::closed, this, &WebSocketCommunicator::onServerClosure);
    }
}

void v3d::dx::WebSocketCommunicator::close()
{
    if (_webSocketServer == nullptr) {
        return;
    }
    _webSocketServer->close();
    _webSocketServer->deleteLater();
    _webSocketServer = nullptr;
}

void v3d::dx::WebSocketCommunicator::connectToRequestSlot(const QObject* _receiver)
{
    const auto* receiver = qobject_cast<const EventQueue*>(_receiver);
    connect(this, &v3d::dx::WebSocketCommunicator::newRequest, receiver, &EventQueue::AddNewRequest);
}

void v3d::dx::WebSocketCommunicator::rpcNotify(QWebSocket *target, const std::string &method, const JsonValue &params)
{
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["method"] = method;
    json["params"] = params;
    // no id; it's a notification
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

void v3d::dx::WebSocketCommunicator::rpcReply(QWebSocket *target, const JsonValue &result, const JsonValue &id)
{
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["result"] = result;
    json["id"] = id;
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

QWebSocket *v3d::dx::WebSocketCommunicator::getClient(int clientId)
{
    return _clients.contains(clientId) ? _clients[clientId] : nullptr;
}

void v3d::dx::WebSocketCommunicator::onNewConnection()
{
    log() << "new connection " << _nextClientId << std::endl;
    QWebSocket *socket = _webSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketCommunicator::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &WebSocketCommunicator::processBinaryMessage);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketCommunicator::onClientClosure);

    _clients.insert(_nextClientId, socket);
    ++_nextClientId;
}

void v3d::dx::WebSocketCommunicator::onServerClosure()
{
    log() << "[Server] Connection closed" << std::endl;
}

void v3d::dx::WebSocketCommunicator::onClientClosure()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    log() << "[Server] Socket disconnected" << std::endl;
    if (client) {
        int key = _clients.key(client, 0);
        if (_clients.contains(key)) { _clients.remove(key); }
        client->deleteLater();
    }
}

void v3d::dx::WebSocketCommunicator::processTextMessage(QString message)
{
    // compute client Id
    auto *client = qobject_cast<QWebSocket *>(sender());
    int clientId = _clients.key(client, 0);
    if (client == nullptr || !_clients.contains(clientId)) {
        log() << "[Server] Message received from invalid client" << std::endl;
        return;
    }
    log() << "[Server] Message received from client " << clientId << ": " << message.toStdString() << std::endl;

    // parse the message into a JSON
    JsonValue json;
    try {
        json = JsonParser().parse(message.toStdString());
    }
    catch (std::exception &) {
        log() << "[Error] Invalid JSON message" << std::endl;
        return;
    }

    // handle different requests
    std::string method = json.get("method", "").toString();
    log() << "message received " << method << std::endl;
    if (method == "queryDatabase") {

        int64_t id = json.get("id", -1).toInt64();
        emit newRequest(clientId, 0, json, [=] (v3d::JsonValue result) {
            log() << "resolved" << std::endl;
            sendDatabase(result, id, clientId);
        });

    }
    else if (method == "openProject") {

        std::string projFileName;
        if (json.contains("params") &&
            json["params"].isObject() &&
            json["params"].contains("fileName") &&
            json["params"]["fileName"].isString())
        {
            projFileName = json["params"]["fileName"].toString();
        }

        emit newRequest(clientId, 1, json, [=] (v3d::JsonValue) {
            notifyProjectOpened(projFileName, clientId);
        });

    }
    else if (method == "closeProject") {

        emit newRequest(clientId, 1, json, [=] (v3d::JsonValue) {
            notifyProjectClosed(clientId);
        });

    }
    else if (method == "getScene") {

        int64_t id = json.get("id", -1).toInt64();
        std::cout << "emitting signals" << std::endl;
        emit newRequest(clientId, 0, json, [=] (v3d::JsonValue scene) {
            sendScene(scene, id, clientId);
        });

    }
    else if (method == "requestFrame") {

        // sample message:
        //  {
        //    "jsonrpc": "2.0",
        //    "method": "requestFrame",
        //    "params": {
        //      "scene": {...}
        //    }
        //  }

//        JsonValue scene;
//        if (json.contains("params") && json["params"].isObject() && json["params"].contains("scene")) {
//            scene = json["params"]["scene"];
//        }
//        emit frameRequested(scene, clientId);
        PING;

    }

//
//    if (method == "queryDatabase") {
//
//        const std::string dsFileName = "./database.json";
//
//        std::ifstream dsFile(dsFileName);
//        if (!dsFile.is_open()) {
//            log() << "[error] cannot open database meta-information file "
//                  << "'"
//                  << dsFileName
//                  << "'" << std::endl;
//            return;
//        }
//        std::string dsString((std::istreambuf_iterator<char>(dsFile)),
//                             std::istreambuf_iterator<char>());
//
//        //log() << dsString << std::endl;
//
//        JsonValue dsJson;
//        try {
//            dsJson = JsonParser().parse(dsString);
//        }
//        catch (std::exception &) {
//            log() << "[error] Invalid JSON file '" << dsFileName << "'" << std::endl;
//            return;
//        }
//
//        log() << "[Database] start reading database meta-info" << std::endl;
//
//        if (dsJson.isArray()) {
//            JsonValue::Array &dsArray = dsJson.toArray();
//            for (auto &ds : dsArray) {
//                //log() << '\t' << ds["data"] << std::endl;
//                QImage img;
//                if (img.load(ds["preview"].toString().c_str())) {
//                    QByteArray ba;
//                    QBuffer buf(&ba);
//                    buf.open(QIODevice::WriteOnly);
//                    img.save(&buf, "JPG");
//                    buf.close();
//                    QByteArray base64 = ba.toBase64();
//                    ds["preview"] = "data:image/jpeg;base64," + base64.toStdString();
//                }
//                else {
//                    log() << "[error] failed to load data preview image "
//                          << "'"
//                          << ds["preview"]
//                          << "'"
//                          << std::endl;
//                    return;
//                }
//            }
//        }
//        else {
//            log() << "[error] wrong database file" << std::endl;
//            return;
//        }
//
//        log() << "[Database] finished reading database meta-info" << std::endl;
//
//        int64_t id = json.get("id", -1).toInt64();
//        rpcReply(client, dsJson, JsonValue(id));
//
//    }
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

void v3d::dx::WebSocketCommunicator::processBinaryMessage(QByteArray message)
{
    log() << "[Server] Binary Message ignored" << std::endl;
}

void v3d::dx::WebSocketCommunicator::notifyProjectOpened(std::string projFileName, int clientId)
{
    log() << "[Server] Project opened: " << projFileName << std::endl;

    QWebSocket *client = getClient(clientId);
    if (client == nullptr)
        return;
    JsonValue params;
    params["fileName"] = projFileName;
    rpcNotify(client, "projectOpened", params);
}

void v3d::dx::WebSocketCommunicator::notifyProjectClosed(int clientId)
{
    log() << "[Server] Project closed" << std::endl;

    QWebSocket *client = getClient(clientId);
    if (client == nullptr)
        return;
    rpcNotify(client, "projectClosed", JsonValue());
}

void v3d::dx::WebSocketCommunicator::sendScene(JsonValue scene, int64_t id, int clientId)
{
    if (!_clients.contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, scene, JsonValue(id));
}

void v3d::dx::WebSocketCommunicator::sendFrame(QImage img, int clientId)
{
    if (!_clients.contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];

    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "JPG");
    buf.close();
    QByteArray base64 = ba.toBase64();

    JsonValue params;
    params["data"] = "data:image/jpeg;base64," + base64.toStdString();

    rpcNotify(client, "frame", params);
}

void v3d::dx::WebSocketCommunicator::sendDatabase(v3d::JsonValue database, int64_t id, int clientId)
{
    if (!_clients.contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, database, JsonValue(id));
}