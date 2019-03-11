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

#include <unistd.h>

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

CPUTaskHandler::CPUTaskHandler(RequestQueue &queue, const std::string &database)
    : TaskHandler(queue)
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
    /**
     * Handle requests in CPU
     * @param id <- client id
     * @param resolve <- respond to client
     * @param json <- request format
     * 
     */
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
    // TODO FIXME busy waiting !!!
    const int err = _queue.dequeueCPU(id, json, resolve);
    if (err == 0) return;

    std::string method = json.get("method", "").toString();

    if (method == "queryDatabase") {

        handle_queryDatabase(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "getScene") {

        handle_getScene(id, resolve, json);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "loadData") {

        std::string projFileName;
        if (json.contains("params") &&
            json["params"].isObject() &&
            json["params"].contains("fileName") &&
            json["params"]["fileName"].isString())
        {
            std::cout << "open project " << projFileName << std::endl;
            projFileName = json["params"]["fileName"].toString();
            handle_loadData(id, projFileName);
        }
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
void CPUTaskHandler::handle_loadData(const clid_t& clientId, const std::string& projectName)
{
    clients::get(clientId)->openProject(projectName);
}

// Delete data after disconnection in CPU
void CPUTaskHandler::handle_delData(const clid_t& clientId)
{
    clients::get(clientId)->closeProject();
}

///////////////////////////////////////////////////////////////////////////////

void GPUTaskHandler::processNextRequest()
{
    /**
     * Handle requests in CPU
     * @param id <- client id
     * @param resolve <- respond to client
     * @param json <- request format
     * 
     */
    clid_t id;
    rply_t resolve;
    json_t json;

    // TODO FIXME busy waiting !!!
    const int err = _queue.dequeueGPU(id, json, resolve);
    if (err == 0) return;

    std::string method = json.get("method", "").toString();

    if (method == "createClient") {

        handle_createClient(id);
        clients::get(id)->incrementCurrCounter();

    } else if (method == "initGL") {

        json_t dummy;
        handle_initOpenGL(id);
        // clients::get(id)->initGL();
        emit onResolve(resolves::add([=]() {
            resolve(dummy);
            log() << "[GPU Task] resolve openProject " << id << std::endl;
        }));
        clients::get(id)->incrementCurrCounter();

    } else if (method == "requestFrame") {

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
        clients::get(id)->incrementCurrCounter();

    } else if(method == "unloadGL") {
        // TODO
        handle_closeOpenGL(id);
        // emit onResolve(resolves::pop([=]() {
        //     log() << "[GPU Task] resolve closeOpenGL" << id << std::endl;
        // }));


        // emit onResolve(resolves::add([=]() {
        //     // resolve(params);
        //     log() << "[GPU Task] resolve closeOpenGL" << id << std::endl;
        // }));
        clients::get(id)->incrementCurrCounter();
    }

}


// Create client 
void GPUTaskHandler::handle_createClient(const clid_t& clientId)
{
    clients::get(clientId)->init(600, 600);
}


// Initialize OpenGL
void GPUTaskHandler::handle_initOpenGL(const clid_t& clientId)
{
    clients::get(clientId)->initGL();
}


// Request Frame
//void GPUTaskHandler::handle_requestFrame(const clid_t& clientId, std::string& img)
//{
//    img = std::move(clients::get(clientId)->renderFrame(scene));
//}


// Close OpenGL
void GPUTaskHandler::handle_closeOpenGL(const clid_t& clientId)
{
    // clients::get(clientId)->closeProject();
    clients::get(clientId)->removeDataFromGPU();
}


}}
