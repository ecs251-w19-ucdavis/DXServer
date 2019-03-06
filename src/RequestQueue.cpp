//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "RequestQueue.h"
#include "Util/Log.h"
#include <queue>

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace queues {

static queues_t global_queue = details::createRequestQueues();

//void create()
//{
//    global_queue.reset(new RequestQueues());
//}

queues_t get()
{
    return global_queue;
}

RequestQueues* raw()
{
    return global_queue.get();
}

}

///////////////////////////////////////////////////////////////////////////////

queues_t details::createRequestQueues()
{
    queues_t ret;
    ret.reset(new RequestQueues());
    return std::move(ret);
}

void RequestQueues::enqueue(std::deque<rqst_t>& queue,
                            const clid_t &client_id,
                            size_t request_id,
                            int type,
                            const json_t &json,
                            const rply_t &resolve)
{
    auto request = requests::create(client_id, request_id, type, json, std::move(resolve));
    auto method = json.get("method", "").toString();
    switch (type) {
        case 0: { // a call
            queue.push_back(request);
            break;
        }
        case 1: { // a notification
//            auto first = std::find_if(queue.begin(), queue.end(), [&](rqst_t &req) {
//                return (req->getClientId() == client_id && req->getType() == type);
//            });
//            if (first != queue.end()) { // if there is a previous notification, replace it with the new one
//                auto it = std::find_if(queue.begin(), first + 1, [&](rqst_t &req) {
//                    const auto req_json = req->getRequest();
//                    return (req_json.get("method", "").toString() == method);
//                });
//                if (it != queue.end()) { // if there is a previous notification, replace it with the new one
//                    *it = request;
//                }
//            }
            queue.push_back(request);
            break;
        }
        default: throw std::runtime_error("[Error] unknown request type");
    }
    log() << "[RQueue] new request received from client " << client_id << ": " << method << std::endl;
}

void RequestQueues::newRequest(clid_t client_id, int type, json_t json, rply_t resolve)
{
    _lock.lock();

    // we create the client if not exist
    auto client = clients::get(client_id);
    if (!client) client = clients::add(client_id);

    // split requests
    std::string method = json.get("method", "").toString();
    if(method == "queryDatabase") {

        auto request_id = client->nextCounterValue(); // I implemented two counters in the Client class
        // each time there is a new request coming in, we get the value of 'next request counter' and
        // then increment the counter's value.
        enqueue(_central_queue, client_id, request_id, type, json, std::move(resolve));

    }
    else if (method == "openProject") {

        // splitting into 2 subrequests:
        // -- createClient enters _graphic_queue
        // -- loadData     enters _central_queue
        // -- initGL       enters _graphic_queue

        const auto request_create_gpu = client -> nextCounterValue();
        const auto request_loadDT_cpu = client -> nextCounterValue();
        const auto request_initGL_gpu = client -> nextCounterValue();
        json_t &json_create_gpu = json;
        json_t  json_loadDT_cpu = json;
        json_t  json_initGL_gpu = json;
        json_create_gpu["method"] = "createClient";
        json_loadDT_cpu["method"] = "loadData";
        json_initGL_gpu["method"] = "initGL";
        std::cout << "gpu task " << json_create_gpu.get("method", "").toString() << std::endl;
        std::cout << "cpu task " << json_loadDT_cpu.get("method", "").toString() << std::endl;
        std::cout << "gpu task " << json_initGL_gpu.get("method", "").toString() << std::endl;
        enqueue(_graphic_queue, client_id, request_create_gpu, type, json_create_gpu, std::move(rply_t{}));
        enqueue(_central_queue, client_id, request_loadDT_cpu, type, json_loadDT_cpu, std::move(rply_t{}));
        enqueue(_graphic_queue, client_id, request_initGL_gpu, type, json_initGL_gpu, resolve);

    }
    else if(method == "closeProject") {

        // splitting into 2 subrequests:
        // -- unloadGL enters _graphic_queue
        // -- delData  enters _central_queue

        const auto request_gpu = client -> nextCounterValue();
        const auto request_cpu = client -> nextCounterValue();
        json_t &json_gpu = json;
        json_t  json_cpu = json;
        json_gpu["method"] = "unloadGL";
        json_cpu["method"] = "delData";
        std::cout << "gpu task" << json_gpu.get("method", "").toString() << std::endl;
        std::cout << "cpu task" << json_cpu.get("method", "").toString() << std::endl;
        enqueue(_graphic_queue, client_id, request_gpu, type, json_gpu, std::move(rply_t{}));
        enqueue(_central_queue, client_id, request_cpu, type, json_cpu, resolve);

    }
    else if (method == "getScene") {

        enqueue(_central_queue, client_id, client->nextCounterValue(), type, json, std::move(resolve));

    }
    else if (method == "requestFrame") {

        enqueue(_graphic_queue, client_id, client->nextCounterValue(), type, json, std::move(resolve));

    }
    else {
        log() << "[Error] Unknown request " << method << std::endl;
    }

    //debugQueue(_central_queue);
    //debugQueue(_graphic_queue);

    _lock.unlock();
}

int RequestQueues::dequeueCPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(_central_queue, client_id, request, resolve);
}

int RequestQueues::dequeueGPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(_graphic_queue, client_id, request, resolve);
}

int RequestQueues::dequeue(std::deque<rqst_t> &queue,
                           clid_t &client_id,
                           json_t &request,
                           rply_t &resolve)
{
    _lock.lock();
    auto it = std::find_if(queue.begin(), queue.end(), [&](rqst_t &req) { return req->isReady(); });
    if (it != queue.end()) {
        client_id = (*it)->getClientId();
        request = (*it)->getRequest();
        resolve = (*it)->getResolve();
        queue.erase(it);
        debugQueue(queue);
        _lock.unlock();
        return 1;
    } else {
        _lock.unlock();
        return 0;
    }
};

void RequestQueues::debugQueue(const std::deque<rqst_t>& queue)
{
    for (const auto& x : queue) {
        auto json = x->getRequest();
        auto id = x->getClientId();
        std::string method = json.get("method", "").toString();
        log() << "\t\t[Debug] new request received from client " << id << ": " << method << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

}}
