//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "Communicator.h"

#include "RequestQueue.h"
#include "Util/Log.h"

#include <QBuffer>

#include <string>

std::string genHash(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string s = "";
    for (int i = 0; i < len; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}

using namespace v3d;

v3d::dx::Communicator::Communicator(quint16 port, QObject *parent)
    : QObject(parent)
    , _port(port)
{
}

void v3d::dx::Communicator::open()
{
    auto sslMode = _secureMode ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode;
    _webSocketServer = new QWebSocketServer(QStringLiteral("Vidi3D Render Server"), sslMode, this);
    if (_webSocketServer->listen(QHostAddress::Any, _port)) {
        log() << ("[RComm] Render server listening on port " + QString("%1").arg(_port, 0, 10)).toStdString()
              << std::endl;
        connect(_webSocketServer, &QWebSocketServer::newConnection, this, &Communicator::onNewConnection);
        connect(_webSocketServer, &QWebSocketServer::closed, this, &Communicator::onServerClosure);
    }
}

void v3d::dx::Communicator::close()
{
    if (_webSocketServer == nullptr) { return; }
    _webSocketServer->close();
    _webSocketServer->deleteLater();
    _webSocketServer = nullptr;
}

void v3d::dx::Communicator::connectToRequestSlot(const QObject* _receiver)
{
    const auto* receiver = qobject_cast<const RequestQueues*>(_receiver);
    connect(this, &Communicator::newRequest, receiver, &RequestQueues::newRequest);
}

void v3d::dx::Communicator::rpcNotify(QWebSocket *target, const std::string &method, const JsonValue &params)
{
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["method"] = method;
    json["params"] = params;
    // no id; it's a notification
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

void v3d::dx::Communicator::rpcReply(QWebSocket *target, const JsonValue &result, const JsonValue &id)
{
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["result"] = result;
    json["id"] = id;
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

bool v3d::dx::Communicator::contains(const clid_t& id) const
{
    return _clients.find(id) != _clients.end();
}

bool v3d::dx::Communicator::getKey(const QWebSocket *socket, clid_t &key) const
{
    auto it = std::find_if (_clients.begin(), _clients.end(), [=](const std::pair<clid_t, QWebSocket*> p) {
        return p.second == socket;
    });
    if (it != _clients.end()) {
        key = it->first;
        return true;
    } else {
        return false;
    }
}

void v3d::dx::Communicator::onNewConnection()
{
    const auto key = genHash(100);
    QWebSocket *socket = _webSocketServer->nextPendingConnection();
    // setup connections
    connect(socket, &QWebSocket::textMessageReceived, this, &Communicator::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &Communicator::processBinaryMessage);
    connect(socket, &QWebSocket::disconnected, this, &Communicator::onClientClosure);
    // save client
    _clients[key] = socket;
    log() << "[RComm] new connection from client " << key << std::endl;
}

void v3d::dx::Communicator::onServerClosure()
{
    log() << "[RComm] Connection closed" << std::endl;
}

void v3d::dx::Communicator::onClientClosure()
{
    auto *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        clid_t key;
        if (getKey(client, key)) { _clients.erase(key); };
        client->deleteLater();
    }
    log() << "[RComm] Socket disconnected" << std::endl;
}

void v3d::dx::Communicator::processTextMessage(QString message)
{
    // compute client Id
    auto *client = qobject_cast<QWebSocket *>(sender());
    clid_t clientId;
    if (client == nullptr || !getKey(client, clientId)) {
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
            log() << "[RComm] inner resolved" << std::endl;
            sendDatabase(result, id, clientId);
        });

    } else if (method == "openProject") {

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

    } else if (method == "closeProject") {

        emit newRequest(clientId, 1, json, [=] (JsonValue) {
            notifyProjectClosed(clientId);
        });

    } else if (method == "getScene") {

        int64_t id = json.get("id", -1).toInt64();
        emit newRequest(clientId, 0, json, [=] (JsonValue scene) {
            sendScene(scene, id, clientId);
        });

    } else if (method == "requestFrame") {

        emit newRequest(clientId, 0, json, [=] (JsonValue frame) {
            sendFrame(frame, clientId);
        });

    } else if (method == "clientKey") {

        log() << "current client key " << JsonParser().stringify(json) << std::endl;
        remapClientKey(client, clientId, json);

    }

}

void v3d::dx::Communicator::processBinaryMessage(QByteArray message)
{
    log() << "[RComm] Binary Message ignored" << std::endl;
}

void v3d::dx::Communicator::notifyProjectOpened(std::string projFileName, clid_t clientId)
{
    log() << "[RComm] Project opened: " << projFileName << std::endl;
    if (!contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    JsonValue params;
    params["fileName"] = projFileName;
    rpcNotify(client, "projectOpened", params);
}

void v3d::dx::Communicator::notifyProjectClosed(clid_t clientId)
{
    log() << "[RComm] Project closed" << std::endl;
    if (!contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcNotify(client, "projectClosed", JsonValue());
}

void v3d::dx::Communicator::sendScene(JsonValue scene, int64_t id, clid_t clientId)
{
    if (!contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, scene, JsonValue(id));
}

void v3d::dx::Communicator::sendFrame(JsonValue params, clid_t clientId)
{
    if (!contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
//    QByteArray base64;
//    {
//        QByteArray ba;
//        QBuffer buf(&ba);
//        buf.open(QIODevice::WriteOnly);
//        img.save(&buf, "JPG");
//        buf.close();
//        base64 = ba.toBase64();
//    }
//    JsonValue params;
//    params["data"] = "data:image/jpeg;base64," + base64.toStdString();
    rpcNotify(client, "frame", params);
}

void v3d::dx::Communicator::sendDatabase(JsonValue database, int64_t id, clid_t clientId)
{
    if (!contains(clientId))
        return;
    QWebSocket *client = _clients[clientId];
    rpcReply(client, database, JsonValue(id));
}

void v3d::dx::Communicator::remapClientKey(QWebSocket* client, clid_t clientId, const JsonValue& data)
{
    std::string key;
    bool old = false;
    if (data.contains("params") &&
        data["params"].isObject() &&
        data["params"].contains("key") &&
        data["params"]["key"].isString())
    {
        key = data["params"]["key"].toString();
        if (clients::has(key)) old = true;
    }
    if (old) {
        // now we should change current client's key
        log() << "found an old client, change its current key back to the old key\n"
              << "(old)\t" << key << std::endl
              << "(new)\t" << clientId << std::endl;
        _clients.erase(clientId);
        _clients[key] = client;
    } else {
        log() << "found a new client " << clientId << std::endl;
        JsonValue json;
        json["key"] = clientId;
        rpcNotify(client, "clientKey", json);
    }
}
