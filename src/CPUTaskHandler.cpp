//
// Created by Qi Wu on 2019-02-23.
//

#include "CPUTaskHandler.h"

#include "Util/Log.h"
#include "Client.h"

#include <QBuffer>
#include <QImage>

namespace v3d { namespace dx {

CPUTaskHandler::CPUTaskHandler(const std::string &database)
{
    loadDatabase(database);
}

void CPUTaskHandler::processNextRequest()
{
    api::client_id_t id;
    api::response_t resolve;
    JsonValue json;

    // get the
    queues::get()->dequeueCPU(id, json, resolve);

    std::string method = json.get("method", "").toString();

    if (method == "queryDatabase") {

        v3d::JsonValue output;
        handleQueryDatabase(id, output);
        resolve(output);

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
        JsonValue::Array &dsArray = _jsonDatabase.toArray();
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
    }
    else {
        throw std::runtime_error("[Error] invalid database lookup file " + database);
    }
}

void CPUTaskHandler::handleQueryDatabase(int clientId, v3d::JsonValue &output)
{
    if (!_jsonDatabase.isNull()) { output = _jsonDatabase; } // make a copy
}

}}