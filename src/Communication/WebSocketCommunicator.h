//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#pragma once
#ifndef V3D_MAIN_SERVER_H
#define V3D_MAIN_SERVER_H

#include "Util/JsonParser.h"

#include "RequestQueue.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QImage>

#include <functional>

/* TODO is there a light weight WebSocket library for doing this ? */
/**
 * This comminicator solves two problems:
 * 1) how to receive requests and pass them to render engine
 * 2) how to retrieve results from the render engine and pass them to web-server
 */
namespace v3d { namespace dx {

class WebSocketCommunicator : public QObject {
    Q_OBJECT
public:
    /**
     * Constructor
     * @param port The network port this communicator will be listening to
     * @param parent Always a nullptr in our application
     */
    explicit WebSocketCommunicator(quint16 port, QObject* parent = nullptr);

    /**
     * Open the communicator and start to listen to connections
     */
    void open();

    /**
     * Terminate the communicator
     */
    void close();

    /**
     * This function is used to connect signals produced by the WebSocketComminicator to a receiver. Such a receiver
     * must be a derived class of QObject. An example implementation of such a receiver can be found in
     * RequestHandler.h and RequestHandler.cpp.
     * @param receiver
     */
    void connectToRequestSlot(const QObject* receiver);

    // enable if needed
    bool isConnected() const { return (_webSocketServer != nullptr) && !_clients.empty(); }

protected:
    void rpcNotify(QWebSocket* target, const std::string& method, const v3d::JsonValue& params);
    void rpcReply(QWebSocket* target, const v3d::JsonValue& result, const v3d::JsonValue& id);
    QWebSocket* getClient(int clientId);

public slots:
    // called by websocket
    void onNewConnection();
    void onServerClosure();
    void onClientClosure();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);

    // call by other components inside this project
    void notifyProjectOpened(std::string projFileName, int clientId);
    void notifyProjectClosed(int clientId);
    void sendScene(v3d::JsonValue scene, int64_t id, int clientId);
    void sendFrame(QImage img, int clientId);
    void sendDatabase(v3d::JsonValue database, int64_t id, int clientId);

signals:
    /**
     * Signal the arrival of a new request. In this function, we should only use pass-by-value rather than
     * pass-by reference because we are async. We don't know if the parent function still exists when running
     * this function.
     * @param clientId
     * @param type The type of a requst: 0 means the request is a call, 1 means the request is a notification.
     * @param request A copy of the original JSON request.
     * @param resolve A functional to send the reply for each request.
     */
    void newRequest(int clientId, int type, v3d::JsonValue request, api::response_t resolve);

private:
    bool _secureMode = false;
    quint16 _port = 8080;
    QWebSocketServer* _webSocketServer = nullptr;
    /// @note we do not need a lock here because all Qt slots are running on the same thread
    /// @note ref: https://doc.qt.io/qt-5/qt.html#ConnectionType-enum
    QHash<int, QWebSocket*> _clients;
    int _nextClientId = 1;
};

}}

#endif // V3D_MAIN_SERVER_H
