//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "WebSocketCommunicator.h"

#include "RequestQueue.h"
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
        log() << ("[RComm] Render server listening on port " + QString("%1").arg(_port, 0, 10)).toStdString()
              << std::endl;
        connect(_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketCommunicator::onNewConnection);
        connect(_webSocketServer, &QWebSocketServer::closed, this, &WebSocketCommunicator::onServerClosure);
    }
}

void v3d::dx::WebSocketCommunicator::close()
{
    if (_webSocketServer == nullptr) { return; }
    _webSocketServer->close();
    _webSocketServer->deleteLater();
    _webSocketServer = nullptr;
}

void v3d::dx::WebSocketCommunicator::connectToRequestSlot(const QObject* _receiver)
{
    const auto* receiver = qobject_cast<const RequestQueues*>(_receiver);
    connect(this, &WebSocketCommunicator::newRequest, receiver, &RequestQueues::EnqueueRequest);
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

QWebSocket *v3d::dx::WebSocketCommunicator::getClient(client_id_t clientId)
{
    return _clients.contains(clientId) ? _clients[clientId] : nullptr;
}

void v3d::dx::WebSocketCommunicator::onNewConnection()
{
    log() << "[RComm] new connection from client " << _nextClientId << std::endl;
    QWebSocket *socket = _webSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketCommunicator::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &WebSocketCommunicator::processBinaryMessage);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketCommunicator::onClientClosure);

    _clients.insert(_nextClientId, socket);
    ++_nextClientId;
}

void v3d::dx::WebSocketCommunicator::onServerClosure()
{
    log() << "[RComm] Connection closed" << std::endl;
}

void v3d::dx::WebSocketCommunicator::onClientClosure()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    log() << "[RComm] Socket disconnected" << std::endl;
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
    client_id_t clientId = _clients.key(client, 0);
    if (client == nullptr || !_clients.contains(clientId)) {
        log() << "[RComm] Message received from invalid client" << std::endl;
        return;
    }
    log() << "[RComm] Message received from client " << clientId << ": " << message.toStdString() << std::endl;

    // parse the message into a JSON
    JsonValue json;
    try {
        json = JsonParser().parse(message.toStdString());
    }
    catch (std::exception &) {
        log() << "[Error] Invalid JSON message: " << __FILE__ << " " << __LINE__ << std::endl;
        return;
    }

    // handle different requests
    std::string method = json.get("method", "").toString();

    if (method == "queryDatabase") {

        int64_t id = json.get("id", -1).toInt64();
        emit newRequest(clientId, 0, json, [=] (JsonValue result) {
            log() << "[RComm] resolved" << std::endl;
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

        emit newRequest(clientId, 1, json, [=] (JsonValue) {
            notifyProjectOpened(projFileName, clientId);
        });

    }
    else if (method == "closeProject") {

        emit newRequest(clientId, 1, json, [=] (JsonValue) {
            notifyProjectClosed(clientId);
        });

    }
    else if (method == "getScene") {

        int64_t id = json.get("id", -1).toInt64();
        std::cout << "[RComm] emitting signals" << std::endl;
        emit newRequest(clientId, 0, json, [=] (JsonValue scene) {
            sendScene(scene, id, clientId);
        });

    }
    else if (method == "requestFrame") {

        PING;

    }

}

void v3d::dx::WebSocketCommunicator::processBinaryMessage(QByteArray message)
{
    log() << "[RComm] Binary Message ignored" << std::endl;
}

void v3d::dx::WebSocketCommunicator::notifyProjectOpened(std::string projFileName, client_id_t clientId)
{
    log() << "[RComm] Project opened: " << projFileName << std::endl;

    QWebSocket *client = getClient(clientId);
    if (client == nullptr)
        return;
    JsonValue params;
    params["fileName"] = projFileName;
    rpcNotify(client, "projectOpened", params);
}

void v3d::dx::WebSocketCommunicator::notifyProjectClosed(client_id_t clientId)
{
    log() << "[RComm] Project closed" << std::endl;
    QWebSocket *client = getClient(clientId);
    if (client == nullptr)
        return;
    rpcNotify(client, "projectClosed", JsonValue());
}

void v3d::dx::WebSocketCommunicator::sendScene(JsonValue scene, int64_t id, client_id_t clientId)
{
    if (!_clients.contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, scene, JsonValue(id));
}

void v3d::dx::WebSocketCommunicator::sendFrame(QImage img, client_id_t clientId)
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

void v3d::dx::WebSocketCommunicator::sendDatabase(JsonValue database, int64_t id, client_id_t clientId)
{
    if (!_clients.contains(clientId)) return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, database, JsonValue(id));
}