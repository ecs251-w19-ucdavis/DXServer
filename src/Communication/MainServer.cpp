#include "MainServer.h"

#include <QBuffer>
#include <QImage>

#include "Util/Log.h"


using namespace v3d;


MainServer::MainServer(quint16 port, QObject *parent)
    : QObject(parent)
    , _port(port)
{
}

void MainServer::open()
{
    auto sslMode = _secureMode ? QWebSocketServer::SecureMode : QWebSocketServer::NonSecureMode;
    _webSocketServer = new QWebSocketServer(QStringLiteral("Vidi3D Render Server"), sslMode, this);
    if (_webSocketServer->listen(QHostAddress::Any, _port)) {
        log() << ("[Server] Render server listening on port " + QString("%1").arg(_port, 0, 10)).toStdString()
              << std::endl;
        connect(_webSocketServer, &QWebSocketServer::newConnection, this, &MainServer::onNewConnection);
        connect(_webSocketServer, &QWebSocketServer::closed, this, &MainServer::onServerClosure);
    }
}

void MainServer::close() {
    if (_webSocketServer == nullptr) {
        return;
    }
    _webSocketServer->close();
    _webSocketServer->deleteLater();
    _webSocketServer = nullptr;
}

void MainServer::sendScene(JsonValue scene, int64_t id, int clientId) {
    if (!_clients.contains(clientId))
        return;
    QWebSocket* client = _clients[clientId];
    rpcReply(client, scene, JsonValue(id));
}

void MainServer::sendFrame(QImage img, int clientId) {
    if (!_clients.contains(clientId))
        return;
    QWebSocket* client = _clients[clientId];

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

void MainServer::test() {
    emit openProjectRequested("D:/Shih/Dropbox/Work/Cpp/V3D/libvidi3d/src/App/build-vidi3d-Desktop_Qt_5_9_1_MSVC2017_64bit-Release/Vorts_test6.json", 0);
}

void MainServer::rpcNotify(QWebSocket* target, const std::string& method, const JsonValue& params) {
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["method"] = method;
    json["params"] = params;
    // no id; it's a notification
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

void MainServer::rpcReply(QWebSocket* target, const JsonValue& result, const JsonValue& id) {
    JsonValue json;
    json["jsonrpc"] = "2.0";
    json["result"] = result;
    json["id"] = id;
    QString msg = QString::fromStdString(JsonParser().stringify(json));
    target->sendTextMessage(msg);
}

QWebSocket* MainServer::getClient(int clientId) {
    return _clients.contains(clientId) ? _clients[clientId] : nullptr;
}

void MainServer::onNewConnection() {
    log() << "new connection" << std::endl;
    QWebSocket* socket = _webSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &MainServer::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &MainServer::processBinaryMessage);
    connect(socket, &QWebSocket::disconnected, this, &MainServer::onClientClosure);

    _clients.insert(_nextClientId, socket);
    ++_nextClientId;
}

void MainServer::onServerClosure() {
    log() << "[Server] Connection closed" << std::endl;
}

void MainServer::onClientClosure() {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    log() << "[Server] Socket disconnected" << std::endl;
    if (client) {

        int key = _clients.key(client, 0);
        if (_clients.contains(key))
            _clients.remove(key);

        client->deleteLater();
    }
}

void MainServer::processTextMessage(QString message) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    int clientId = _clients.key(client, 0);

    if (client == nullptr || !_clients.contains(clientId)) {
        log() << "[Server] Message received from invalid client" << std::endl;
        return;
    }

    log() << "[Server] Message received from client " << clientId << ": " << message.toStdString() << std::endl;

    JsonValue json;
    try {
        json = JsonParser().parse(message.toStdString());
    } catch (std::exception&) {
        log() << "[error] Invalid JSON message" << std::endl;
        return;
    }

    std::string method = json.get("method", "").toString();

    log() << "message received " << method << std::endl;

    if (method == "queryDatabase") {

	const std::string dsFileName = "./database.json";

	std::ifstream dsFile(dsFileName);
	if (!dsFile.is_open()) {
	    log() << "[error] cannot open database meta-information file "
		  << "'"
		  << dsFileName 
		  << "'" << std::endl;
	    return;
	}
	std::string dsString((std::istreambuf_iterator<char>(dsFile)), 
			     std::istreambuf_iterator<char>());

	//log() << dsString << std::endl;

	JsonValue dsJson;
	try {
	    dsJson = JsonParser().parse(dsString);
	} catch (std::exception&) {
	    log() << "[error] Invalid JSON file '" << dsFileName << "'" << std::endl;
	    return;
	}
	
	log() << "[Database] start reading database meta-info" << std::endl;

	if (dsJson.isArray()) {
	    JsonValue::Array &dsArray = dsJson.toArray();
	    for (auto &ds : dsArray) {
	        //log() << '\t' << ds["data"] << std::endl;
		QImage img;
		if (img.load(ds["preview"].toString().c_str())) {
		    QByteArray ba;
		    QBuffer buf(&ba);
		    buf.open(QIODevice::WriteOnly);
		    img.save(&buf, "JPG");
		    buf.close();
		    QByteArray base64 = ba.toBase64();  
		    ds["preview"] = "data:image/jpeg;base64," + base64.toStdString();
		} else {
		    log() << "[error] failed to load data preview image "
			  << "'"
			  << ds["preview"] 
			  << "'"
			  << std::endl;
		    return;
		}
	    }
	} else {
	    log() << "[error] wrong database file" << std::endl;
	    return;
	}

	log() << "[Database] finished reading database meta-info" << std::endl;

	int64_t id = json.get("id", -1).toInt64();
	rpcReply(client, dsJson, JsonValue(id));

    } else if (method == "openProject") {

        std::string projFileName;
        if (json.contains("params") &&
                json["params"].isObject() &&
                json["params"].contains("fileName") &&
                json["params"]["fileName"].isString()) {
            projFileName = json["params"]["fileName"].toString();
        }
        if (!projFileName.empty()) {
            emit openProjectRequested(projFileName, clientId);
        }

    } else if (method == "closeProject") {

        emit closeProjectRequested(clientId);

    } else if (method == "getScene") {

        int64_t id = json.get("id", -1).toInt64();
        emit getSceneRequested(id, clientId);

    } else if (method == "requestFrame") {

        // sample message:
        //  {
        //    "jsonrpc": "2.0",
        //    "method": "requestFrame",
        //    "params": {
        //      "scene": {...}
        //    }
        //  }
        JsonValue scene;
        if (json.contains("params") && json["params"].isObject() && json["params"].contains("scene")) {
            scene = json["params"]["scene"];
        }
        emit frameRequested(scene, clientId);

    }
}

void MainServer::processBinaryMessage(QByteArray message) {
    log() << "[Server] Binary Message ignored" << std::endl;
}

void MainServer::notifyProjectOpened(std::string projFileName, int clientId) {
    log() << "[Server] Project opened: " << projFileName << std::endl;

    QWebSocket* client = getClient(clientId);
    if (client == nullptr)
        return;
    JsonValue params;
    params["fileName"] = projFileName;
    rpcNotify(client, "projectOpened", params);
}

void MainServer::notifyProjectClosed(int clientId) {
    log() << "[Server] Project closed" << std::endl;

    QWebSocket* client = getClient(clientId);
    if (client == nullptr)
        return;
    rpcNotify(client, "projectClosed", JsonValue());
}
