#ifndef V3D_MAIN_SERVER_H_
#define V3D_MAIN_SERVER_H_

#include <QWebSocket>
#include <QWebSocketServer>

#include "Util/JsonParser.h"

/* TODO is there a light weight WebSocket library for doing this ? */
/**
 * This comminicator solves two problems:
 * 1) how to receive requests and pass them to render engine
 * 2) how to retrieve results from the render engine and pass them to web-server
 */
class WebSocketCommunicator : public QObject {
    Q_OBJECT
public:
    explicit WebSocketCommunicator(quint16 port, QObject* parent = nullptr);

    void open();
    void close();

    void sendScene(v3d::JsonValue scene, int64_t id, int clientId);
    void sendFrame(QImage img, int clientId);

    bool isConnected() const { return (_webSocketServer != nullptr) && !_clients.empty(); }

    void test();

protected:
    void rpcNotify(QWebSocket* target, const std::string& method, const v3d::JsonValue& params);
    void rpcReply(QWebSocket* target, const v3d::JsonValue& result, const v3d::JsonValue& id);
    QWebSocket* getClient(int clientId);

public slots:
    void onNewConnection();
    void onServerClosure();
    void onClientClosure();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);

    void notifyProjectOpened(std::string projFileName, int clientId);
    void notifyProjectClosed(int clientId);

signals:
    void openProjectRequested(std::string projFileName, int clientId);
    void closeProjectRequested(int clientId);
    void getSceneRequested(int64_t id, int clientId);
    void frameRequested(v3d::JsonValue scene, int clientId);

private:
    bool _secureMode = false;
    quint16 _port = 8080;
    QWebSocketServer* _webSocketServer = nullptr;
    QHash<int, QWebSocket*> _clients;
    int _nextClientId = 1;
};

#endif // V3D_MAIN_SERVER_H_
