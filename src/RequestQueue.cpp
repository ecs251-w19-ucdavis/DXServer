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

static v3d::dx::queues_t global_queue;

namespace v3d { namespace dx {

///////////////////////////////////////////////////////////////////////////////

namespace queues {

void create()
{
    global_queue.reset(new RequestQueues());
}

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

Request::Request(clid_t id, clid_t exp, int type, json_t request, rply_t resolve)
    : _id(id)
    , _exp(exp)
    , _type(type)
    , _request(std::move(request))
    , _resolve(std::move(resolve))
{}

void RequestQueues::EnqueueRequest(clid_t client_id, int type, json_t json, rply_t resolve)
{
    // TODO should add lock -- DONE
    // TODO should decide which queue to add
    // TODO should compute expected request id for each request -- DONE
    _lock.lock();

    // we create the client if not exist
    auto client = clients::get(client_id);
    if (!client) client = clients::add(client_id);

    auto request_id = client->nextCounterValue(); // I implemented two counters in the Client class
    // each time there is a new request coming in, we get the value of 'next request counter' and then increment the
    // counter's value.
    auto request = std::make_shared<Request>(client_id, request_id, type, json, std::move(resolve));

    std::string method = json.get("method", "").toString();
    log() << "[RQueue] new request received from client " << client_id << ": " << method << std::endl;

    switch (type) {
        case 0: { // a call
            QueueCPU.push_back(request);
            break;
        }
        case 1: { // a notification
            auto it = std::find_if(QueueCPU.begin(), QueueCPU.end(), [&](reqt_t &req) {
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

    debugQueue(QueueCPU);

    _lock.unlock();
}

int RequestQueues::dequeueCPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(QueueCPU, client_id, request, resolve);
}

int RequestQueues::dequeueGPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(QueueGPU, client_id, request, resolve);
}

int RequestQueues::dequeue(std::deque<reqt_t> &queue,
                           clid_t &client_id,
                           json_t &request,
                           rply_t &resolve)
{
    _lock.lock();
    auto it = std::find_if(queue.begin(), queue.end(), [&](reqt_t &req) { return req->isReady(); });
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

void RequestQueues::debugQueue(const std::deque<reqt_t>& queue)
{
    for (const auto& x : queue) {
        auto json = x->getRequest();
        auto id = x->getClientId();
        std::string method = json.get("method", "").toString();
        log() << "[Debug] new request received from client " << id << ": " << method << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

}}
