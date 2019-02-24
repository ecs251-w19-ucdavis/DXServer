#ifndef V3D_MAIN_SERVER_H_
#define V3D_MAIN_SERVER_H_

#include "Util/JsonParser.h"

#include <QWebSocket>
#include <QWebSocketServer>

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
//    using RequestSlot = std::function<void(int, int, v3d::JsonValue, const std::function<void(v3d::JsonValue)>&)>;
public:
    explicit WebSocketCommunicator(quint16 port, QObject* parent = nullptr);

    void open();
    void close();

    /**
     * This function is used to connect signals produced by the WebSocketComminicator to a receiver. Such a receiver
     * must be a derived class of QObject. An example implementation of such a receiver can be found in
     * RequestHandler.h and RequestHandler.cpp.
     * @param receiver
     */
    void connectToRequestSlot(const QObject* receiver);

//    bool isConnected() const { return (_webSocketServer != nullptr) && !_clients.empty(); }

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
    void sendScene(v3d::JsonValue scene, int64_t id, int clientId);
    void sendFrame(QImage img, int clientId);
    void sendDatabase(v3d::JsonValue database, int64_t id, int clientId);

signals:
    /**
     * Signal the arrival of a new request
     * @param clientId
     * @param type The type of a requst: 0 means the request is a call, 1 means the request is a notification
     * @param request
     * @param resolve
     */
    void newRequest(int clientId, int type, v3d::JsonValue request, const std::function<void(v3d::JsonValue)>& resolve);
//    void openProjectRequested(std::string projFileName, int clientId);
//    void closeProjectRequested(int clientId);
//    void getSceneRequested(int64_t id, int clientId);
//    void frameRequested(v3d::JsonValue scene, int clientId);

private:
    bool _secureMode = false;
    quint16 _port = 8080;
    QWebSocketServer* _webSocketServer = nullptr;

    // TODO there is no lock for this ?
    QHash<int, QWebSocket*> _clients;
    int _nextClientId = 1;

};

}}

#endif // V3D_MAIN_SERVER_H_
