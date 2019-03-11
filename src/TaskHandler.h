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
#include <condition_variable>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

/**
 * This class suppose to handle requests from the event queue. This class will
 * run in a different thread.
 */
class TaskHandler : public QObject {
    Q_OBJECT
    using thread_t = std::shared_ptr<std::thread>;
public:
    explicit TaskHandler() = default;
    void connectToRequestQueue(std::shared_ptr<RequestQueue> queue);
    void connectToCommunicator(std::shared_ptr<Communicator> receiver);

    virtual void processNextRequest() = 0;
    void run() {
        if (!_queue) {
            throw std::runtime_error("[Error] request queue hasn't been created.");
        }
        while (true) processNextRequest();
    }

    void setPoolSize(size_t n) {
        _size = n;
        _pool.resize(n);
    }

    void signal() {
        _cv_var.notify_one();
    }

    template<class Pred>
    void wait(const Pred& p) {

    }

signals:
    void onResolve(int);

protected:
    std::shared_ptr<RequestQueue> _queue;

    std::vector<thread_t> _pool;
    size_t                _size = 1;

    std::condition_variable _cv_var;
    std::mutex              _cv_lck;
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

    /**
     * Query database in CPU
     * @param id
     * @param resolve
     * @param json
     */
    void handle_queryDatabase(const clid_t& id, const rply_t& resolve, const json_t& json);

    /**
     * Get scene in CPU
     * @param id
     * @param resolve
     * @param json
     */
    void handle_getScene(const clid_t& id, const rply_t& resolve, const json_t& json);

    /**
     * Load data in CPU
     * @param id
     * @param resolve
     * @param json
     */
    void handle_loadData(const clid_t& id, const rply_t& resolve, const json_t& json);

    /**
     * Delete data after disconnection in CPU
     * @param id
     * @param resolve
     * @param json
     */
    void handle_delData(const clid_t& id, const rply_t& resolve, const json_t& json);

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
    *
    * @param id
    * @param resolve
    * @param json
    */
    void handle_initOpenGL(const clid_t& id, const rply_t& resolve, const json_t& json);

   /**
    *
    * @param id
    * @param resolve
    * @param json
    */
    void handle_requestFrame(const clid_t& id, const rply_t& resolve, const json_t& json);

   /**
    * @param id
    * @param resolve
    * @param json
    */
    void handle_closeOpenGL(const clid_t& id, const rply_t& resolve, const json_t& json);
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
