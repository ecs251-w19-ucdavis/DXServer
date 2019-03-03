//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

#include "RequestQueue.h"
#include <queue>

static v3d::dx::api::queues_t global_queue;
namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace queues {

void create()
{
    global_queue = std::make_shared<RequestQueues>();
}

api::queues_t get()
{
    return global_queue;
}

RequestQueues* raw()
{
    return global_queue.get();
}

}

///////////////////////////////////////////////////////////////////////////////

Request::Request(api::client_id_t id, api::client_id_t exp, int type, api::json_t request, api::response_t resolve)
    : _id(id)
    , _exp(exp)
    , _type(type)
    , _request(std::move(request))
    , _resolve(std::move(resolve))
{}

void RequestQueues::EnqueueRequest(api::client_id_t client_id, int type, api::json_t json, api::response_t resolve)
{
    // TODO should add lock -- DONE
    // TODO should decide which queue to add
    // TODO should compute expected request id for each request -- DONE
    _lock.lock();

    auto request_id = clients::get(client_id)->nextCounterValue(); // I implemented two counters in the Client class
    // each time there is a new request coming in, we get the value of 'next request counter' and then increment the
    // counter's value.
    auto request = std::make_shared<Request>(client_id, request_id, type, json, std::move(resolve));

    std::string method = json.get("method", "").toString();
    std::cout << "new request received from client " << client_id << ": " << method << std::endl;

    switch (type) {
        case 0: { // a call
            QueueCPU.push_back(request);
            break;
        }
        case 1: { // a notification
            auto it = std::find_if(QueueCPU.begin(), QueueCPU.end(), [&](api::request_t &req) {
                return (req->getClientId() == client_id && req->getType() == type);
            });
            if (it != QueueCPU.end()) { // if there is a previous notification, replace it with the new one
                *it = request;
            } else {
                QueueCPU.push_back(request);
            }
            break;
        }
        default: throw std::runtime_error("[Error] unknown request type");
    }

    _lock.unlock();
}

int RequestQueues::dequeueCPU(api::client_id_t &client_id,
                              api::json_t& request,
                              api::response_t& resolve)
{
    return dequeue(QueueCPU, client_id, request, resolve);
}

int RequestQueues::dequeueGPU(api::client_id_t &client_id,
                              api::json_t& request,
                              api::response_t& resolve)
{
    return dequeue(QueueGPU, client_id, request, resolve);
}

int RequestQueues::dequeue(std::deque<api::request_t> &queue,
                           api::client_id_t &client_id,
                           api::json_t &request,
                           api::response_t &resolve)
{
    _lock.lock();
    auto it = std::find_if(queue.begin(), queue.end(), [&](api::request_t &req) { return req->isReady(); });
    if (it != queue.end()) {
        client_id = (*it)->getClientId();
        request = (*it)->getRequest();
        resolve = (*it)->getResolve();
        _lock.unlock();
        return 1;
    } else {
        _lock.unlock();
        return 0;
    }
};

///////////////////////////////////////////////////////////////////////////////

}}
