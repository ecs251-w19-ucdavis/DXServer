//
// Created by Qi Wu on 2019-02-23.
//

#pragma once
#ifndef DXSERVER_REQUESTHANDLER_H
#define DXSERVER_REQUESTHANDLER_H

#include "RequestQueue.h"
#include "Communicator.h"
#include "Util/JsonParser.h"

#include <QThread>
#include <functional>

namespace v3d { namespace dx {

/**
 * This class suppose to handle requests from the event queue. This class will run in a different thread.
 */
class CPUTaskHandler : public QThread {
    Q_OBJECT
public:
    explicit CPUTaskHandler(const std::string& database = "database.json");
    void run() override { while (true) processNextRequest(); }
    void processNextRequest();
    void connectToCommunicator(const QObject *_receiver);

private:
    void loadDatabase(const std::string& database);
    void handleQueryDatabase(clid_t clientId, json_t& output);

signals:
    void onResolve(int);

private:
    json_t _jsonDatabase; // here we cache the database file to avoid reloading
};

}}


#endif //DXSERVER_REQUESTHANDLER_H
