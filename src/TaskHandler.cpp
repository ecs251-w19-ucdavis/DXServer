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

#include <mutex>

///////////////////////////////////////////////////////////////////////////////

/** Here are the part of requests we deal with in CPU.
 * First, we Load database from disk, and read/query data from the database.
 * Second, We remove database from CPU.
 * Finally, we close clients' connection and delete the clients.
 * 
 */
namespace v3d { namespace dx {

void TaskHandler::connectToRequestQueue(std::shared_ptr<RequestQueue> queue)
{
    _queue = std::move(queue);
}

void TaskHandler::connectToCommunicator(std::shared_ptr<Communicator> receiver)
{
    connect(this, &TaskHandler::onResolve, receiver.get(), &Communicator::onResolve);
}

///////////////////////////////////////////////////////////////////////////////

CPUTaskHandler::CPUTaskHandler(const std::string &database)
    : TaskHandler()
{
    // Load database from disk
    loadDatabase(database);
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

void CPUTaskHandler::processNextRequest()
{
    // variables from a request
    clid_t id; // client id
    rply_t resolve; //< respond to client
    json_t json; //< request format

    /*
     * Get the data from the RequestQueue, since we split the openProject request into two parts:
     *  1. load data (in CPU)
     *  2. initilize OpenGL (in GPU)
     * 
     *  Here in CPU we deal with the load data request
     */

    // TODO FIXME busy waiting !!!
    const int err = _queue->dequeueCPU(id, json, resolve);
    if (err == 0) return;

    // check each request
    std::string method = json.get("method", "").toString();
    if (method == "queryDatabase") {

        handle_queryDatabase(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "getScene") {

        handle_getScene(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "loadData") {

        handle_loadData(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else {
        std::cout << "unknown method " << method << std::endl;
    }

}

// Query database in CPU
void CPUTaskHandler::handle_queryDatabase(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    json_t output;
    // make a copy of the cached database if exists
    if (!_jsonDatabase.isNull()) { output = _jsonDatabase; } // make a copy
    // this is used to respond to client
    emit onResolve(resolves::add([=]() {
        resolve(output);
        log() << "[CPU Task] resolve queryDatabase " << id << std::endl;
    }));
}

// Get scene in CPU
void CPUTaskHandler::handle_getScene(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    json_t output;
    output = std::move(clients::get(id)->getScene());
    emit onResolve(resolves::add([=]() {
        resolve(output);
        log() << "[CPU Task] resolve getScene " << id << std::endl;
    }));
}

// Load data in CPU
void CPUTaskHandler::handle_loadData(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    std::string projFileName;
    if (json.contains("params") &&
        json["params"].isObject() &&
        json["params"].contains("fileName") &&
        json["params"]["fileName"].isString())
    {
        std::cout << "open project " << projFileName << std::endl;
        projFileName = json["params"]["fileName"].toString();
        clients::get(id)->openProject(projFileName, 600, 600);
    }
}

// Delete data after disconnection in CPU
void CPUTaskHandler::handle_delData(const clid_t& id, const rply_t& resolve, const json_t& json)
{
//    clients::get(id)->closeProject();
}

///////////////////////////////////////////////////////////////////////////////

void GPUTaskHandler::processNextRequest()
{
    // variables from a request
    clid_t id; // client id
    rply_t resolve; //< respond to client
    json_t json; //< request format

    // TODO FIXME busy waiting !!!
    const int err = _queue->dequeueGPU(id, json, resolve);
    if (err == 0) return;

    // check each request
    std::string method = json.get("method", "").toString();
    if (method == "initGL") {

        handle_initOpenGL(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "requestFrame") {

        handle_requestFrame(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if(method == "unloadGL") {

        handle_closeOpenGL(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    }

}

// Initialize OpenGL
void GPUTaskHandler::handle_initOpenGL(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    json_t dummy;
    clients::get(id)->initGL();
    emit onResolve(resolves::add([=]() {
        resolve(dummy);
        log() << "[GPU Task] resolve openProject " << id << std::endl;
    }));
}

// Request Frame
void GPUTaskHandler::handle_requestFrame(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    JsonValue scene;
    if (json.contains("params") &&
        json["params"].isObject() &&
        json["params"].contains("scene"))
    {
        scene = json["params"]["scene"];
    }
    JsonValue params = clients::get(id)->renderFrame(scene);
    emit onResolve(resolves::add([=]() {
        resolve(params);
        log() << "[GPU Task] resolve requestFrame " << id << std::endl;
    }));
}

// Close OpenGL
void GPUTaskHandler::handle_closeOpenGL(const clid_t& id, const rply_t& resolve, const json_t& json)
{
    // clients::get(clientId)->closeProject();
//    clients::get(id)->removeDataFromGPU();
}


}}
