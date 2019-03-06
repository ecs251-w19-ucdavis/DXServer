//
// Created by Qi Wu on 2019-02-23.
//

#include "TaskHandler.h"

#include "Communicator.h"
#include "Util/Resolve.h"
#include "Util/Client.h"

#include "Util/Log.h"

#include <QBuffer>
#include <QImage>

///////////////////////////////////////////////////////////////////////////////

/** Here are the part of requests we deal with in CPU.
 * First, we Load database from disk, and read/query data from the database.
 * Second, We remove database from CPU.
 * Finally, we close clients' connection and delete the clients.
 * 
 */
namespace v3d { namespace dx {

void TaskHandler::connectToCommunicator(const QObject *_receiver)
{
    const auto* receiver = qobject_cast<const Communicator*>(_receiver);
    connect(this, &TaskHandler::onResolve, receiver, &Communicator::onResolve);
}

///////////////////////////////////////////////////////////////////////////////

CPUTaskHandler::CPUTaskHandler(const std::string &database)
    : TaskHandler()
{
    // Load database from disk
    loadDatabase(database);
}

void CPUTaskHandler::processNextRequest()
{
    clid_t id;
    rply_t resolve;
    json_t json;

    /** get the data from the RequestQueue, since we split the openProject request into two parts:
     *  1. load data (in CPU)
     *  2. initilize OpenGL (in GPU)
     * 
     *  Here in CPU we deal with the load data request  
     * 
    */
    // get the
    if (!queues::get()->dequeueCPU(id, json, resolve)) return;

    std::string method = json.get("method", "").toString();

    if (method == "queryDatabase") {

        json_t output;
        handle_QueryDatabase(id, output);
        emit onResolve(resolves::add([=]() {
            resolve(output);
            log() << "[CPU] resolve " << id << std::endl;
        }));
        clients::get(id)->incrementCurrCounter();

    } else {

    }
}

void CPUTaskHandler::loadDatabase(const std::string& database)
{
    // open the database file as a string
    std::ifstream dsFile(database);
    if (!dsFile.is_open()) {
        // I want to avoid crashing simply because database.json does not exists.
        log() << "[error] cannot open database lookup file "
              << "'"
              << database
              << "' please generate one by re-run cmake"
              << std::endl;
        return;
    }
    std::string dsString((std::istreambuf_iterator<char>(dsFile)), std::istreambuf_iterator<char>());

    // convert the string into JSON
    try { _jsonDatabase = JsonParser().parse(dsString); }
    catch (std::exception &) {
        throw std::runtime_error("[Error] invalid database lookup file " + database);
    }

    // convert preview image into base64 string
    if (_jsonDatabase.isArray()) {
        json_t::Array &dsArray = _jsonDatabase.toArray();
        for (auto &ds : dsArray) {
            QImage img;
            if (img.load(ds["preview"].toString().c_str())) {
                QByteArray ba;
                QBuffer buf(&ba);
                buf.open(QIODevice::WriteOnly);
                img.save(&buf, "JPG");
                buf.close();
                QByteArray base64 = ba.toBase64();
                ds["preview"] = "data:image/jpeg;base64," + base64.toStdString();
            }
            else {
                throw std::runtime_error("[Error] failed to load data preview image " + ds["preview"].toString());
            }
        }
    } else {
        throw std::runtime_error("[Error] invalid database lookup file " + database);
    }
}

// Read Data From Database
void CPUTaskHandler::handle_QueryDatabase(clid_t clientId, json_t &output)
{
    if (!_jsonDatabase.isNull()) { output = _jsonDatabase; } // make a copy
}

///////////////////////////////////////////////////////////////////////////////



}}