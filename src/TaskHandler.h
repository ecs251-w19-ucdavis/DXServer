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
    explicit TaskHandler(RequestQueues & q) : _queue(q) {}

    virtual void processNextRequest() = 0;
    void run() { while (true) processNextRequest(); }
    void connectToCommunicator(const QObject *_receiver);

    void setPoolSize(size_t n) {
        _size = n;
        _pool.resize(n);
    }

signals:
    void onResolve(int);
protected:
    RequestQueues &_queue;
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
    explicit CPUTaskHandler(RequestQueues &queue, const std::string& database = "database.json");
    void processNextRequest() override;

private:
    void loadDatabase(const std::string& database);

    /**
     * Query database in CPU
     * @param id
     * @param resolve
     * @param json
     */
    void handle_queryDatabase(const clid_t& id, const rply_t& resolve, const json_t& json);

    /**
     *
     * @param id
     * @param resolve
     * @param json
     */
    void handle_getScene(const clid_t& id, const rply_t& resolve, const json_t& json);

    /**
     *
     * @param id
     * @param projectName
     */
    void handle_loadData(const clid_t& id, const std::string& projectName);

    /**
     *
     * @param id
     */
    void handle_delData(const clid_t& id);

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
    explicit GPUTaskHandler(RequestQueues & queue) : TaskHandler(queue) {}
    void processNextRequest() override;

private:
/**
 * Create client
 * Requset: OpenProject, initialize OpenGL, pass data to GPU 
 * Request: RequestFrame
 * Request: QuerryData
 * Request: CloseProject, clean OpenGL
 */


   /**
   * Query database in GPU
   * @param id
   */
    void handle_createClient(const clid_t& id);
    
   /**
   * @param id
   * @param resolve
   * @param json
   */
    void handle_initOpenGL(const clid_t& id, const rply_t& resolve, const json_t& json);

   /**
   * @param id
   * @param resolve
   * @param json
   */
    void handle_requestFrame(const clid_t& id, const rply_t& resolve, const json_t& json);

   /**
   * @param id
   */
    void handle_closeOpenGL(const clid_t& id);
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
