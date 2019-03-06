//
// Created by Qi Wu on 2019-02-23.
//

#pragma once
#ifndef DXSERVER_REQUESTHANDLER_H
#define DXSERVER_REQUESTHANDLER_H

#include "RequestQueue.h"
#include "Communicator.h"

#include "Util/JsonParser.h"

#include <QObject>

#include <functional>
#include <vector>
#include <thread>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

/**
 * This class suppose to handle requests from the event queue. This class will run in a different thread.
 */
class TaskHandler : public QObject {
    Q_OBJECT
    using thread_t = std::shared_ptr<std::thread>;
public:
    TaskHandler() = default;
    ~TaskHandler() override = default;
    virtual void processNextRequest() = 0;
    void run() { while (true) processNextRequest(); }
    void connectToCommunicator(const QObject *_receiver);

    void setPoolSize(size_t n) {
        _size = n;
        _pool.resize(n);
    }

signals:
    void onResolve(int);

private:
    std::vector<thread_t> _pool;
    size_t                _size = 1;
};

///////////////////////////////////////////////////////////////////////////////

/**
 *
 */
class CPUTaskHandler : public TaskHandler {
    Q_OBJECT
public:
    explicit CPUTaskHandler(const std::string& database = "database.json");
    void processNextRequest() override;

private:
    void loadDatabase(const std::string& database);
    void handle_queryDatabase(clid_t clientId, json_t &output);
    void handle_getScene(clid_t clientId, json_t &output);

private:
    json_t _jsonDatabase; // here we cache the database file to avoid reloading
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This class suppose to handle requests from the event queue. This class will open and run in a different thread.
 */
class GPUTaskHandler : public TaskHandler {
    Q_OBJECT
public:
    GPUTaskHandler() = default;
    void processNextRequest() override {};

private:
    void handle_initOpenGL();
    void handle_requestFrame();

    void handle_queryDatabase();
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
