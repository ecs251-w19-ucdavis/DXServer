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

#include "RequestQueue.h"
#include "Util/Resolve.h"

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

class Communicator : public QObject {
    Q_OBJECT
public:
    /**
     * Constructor
     * @param port The network port this communicator will be listening to
     * @param parent Always a nullptr in our application
     */
    explicit Communicator(quint16 port, QObject* parent = nullptr);

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

    /**
     * Check if there is at least one connection.
     */
    bool isConnected() const { return (_webSocketServer != nullptr) && !_clients.empty(); }

protected:
    // basic functionality
    void rpcNotify(QWebSocket* target, const std::string& method, const JsonValue& params);
    void rpcReply(QWebSocket* target, const JsonValue& result, const JsonValue& id);
    QWebSocket* getClient(clid_t clientId);

    // call by onResolve
    void notifyProjectOpened(std::string projFileName, clid_t clientId);
    void notifyProjectClosed(clid_t clientId);
    void sendScene(JsonValue scene, int64_t id, clid_t clientId);
    void sendFrame(QImage img, clid_t clientId);
    void sendDatabase(JsonValue database, int64_t id, clid_t clientId);

public slots:
    // called by websocket
    void onNewConnection();
    void onServerClosure();
    void onClientClosure();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);

    // call by our server
    void onResolve(int id) { replies::get(id)(); };

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
    void newRequest(clid_t clientId, int type, json_t request, rply_t resolve);

private:
    bool _secureMode = false;
    quint16 _port = 8080;
    QWebSocketServer* _webSocketServer = nullptr;
    /// @note we do not need a lock here because all Qt slots are running on the same thread
    /// @note ref: https://doc.qt.io/qt-5/qt.html#ConnectionType-enum
    QHash<clid_t, QWebSocket*> _clients;
    clid_t _nextClientId = 1;
};

}}

#endif // V3D_MAIN_SERVER_H
