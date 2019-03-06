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

void RequestQueues::enqueue(clid_t client_id, size_t request_id, int type, json_t json, rply_t resolve)
{
    auto request = requests::create(client_id, request_id, type, json, std::move(resolve));
    std::string method = json.get("method", "").toString();
    log() << "[RQueue] new request received from client " << client_id << ": " << method << std::endl;
    switch (type) {
        case 0: { // a call
            _queue_cpu.push_back(request);
            break;
        }
        case 1: { // a notification
            if(method == "LoadData" || method == "DelData") // CPU notifications
            {
                auto it = std::find_if(_queue_cpu.begin(), _queue_cpu.end(), [&](rqst_t &req) {
                    return (req->getClientId() == client_id && req->getType() == type);
                });
                if (it != _queue_cpu.end()) { // if there is a previous notification, replace it with the new one
                    *it = request;
                } else {
                    _queue_cpu.push_back(request);
                }
                break;
            }
            else // GPU notifications
            {
                auto it = std::find_if(_queue_gpu.begin(), _queue_gpu.end(), [&](rqst_t &req) {
                    return (req->getClientId() == client_id && req->getType() == type);
                });
                if (it != _queue_gpu.end()) { // if there is a previous notification, replace it with the new one
                    *it = request;
                } else {
                    _queue_gpu.push_back(request);
                }
                break;
            }
        }
        default: throw std::runtime_error("[Error] unknown request type");
    }
}

void RequestQueues::enqueueRequest(clid_t client_id, int type, json_t json, rply_t resolve)
{
    // TODO should add lock -- DONE
    // TODO should decide which queue to add
    // TODO should compute expected request id for each request -- DONE
    _lock.lock();

    std::string method = json.get("method", "").toString();        
    // we create the client if not exist
    auto client = clients::get(client_id);
    if (!client) client = clients::add(client_id);

    if(method == "openProject") // splitting into 2 subrequests: LoadData enters _queue_cpu, and InitGL enters _queue_gpu
    {
        auto request_id1 = client -> nextCounterValue(),
             request_id2 = client -> nextCounterValue();
        json_t json1 = json;
        json1["method"] = "LoadData";
        json_t json2 = json;
        json2["method"] = "InitGL";
        std::cout << "json1" << json1.get("method", "").toString() << std::endl;
        std::cout << "json2" << json2.get("method", "").toString() << std::endl;
        // rply_t resolve1 = std::function<void(JsonValue)>{};
        enqueue(client_id, request_id1, type, json1, std::move(rply_t{}));
        enqueue(client_id, request_id2, type, json2, std::move(resolve));
    }
    else if(method == "closeProject") //splitting into 2 subrequests: LoadData enters _queue_cpu, and InitGL enters _queue_gpu
    {
        auto request_id1 = client -> nextCounterValue(),
             request_id2 = client -> nextCounterValue();
        json_t json1 = json;
        json1["method"] = "DelData";
        json_t json2 = json;
        json2["method"] = "CloseGL";
        std::cout << "json1" << json1.get("method", "").toString() << std::endl;
        std::cout << "json2" << json2.get("method", "").toString() << std::endl;
        // rply_t resolve1 = std::function<void(JsonValue)>{};
        enqueue(client_id, request_id1, type, json1, std::move(rply_t{}));
        enqueue(client_id, request_id2, type, json2, std::move(resolve));
    }
    else
    {
        auto request_id = client->nextCounterValue(); // I implemented two counters in the Client class
        // each time there is a new request coming in, we get the value of 'next request counter' and
        // then increment the counter's value.
        std::cout << "curr counter " << client->currCounterValue() << " next " << request_id << std::endl;
        enqueue(client_id, request_id, type, json, std::move(resolve));
    }

    //debugQueue(_queue_cpu);
    //debugQueue(_queue_gpu);

    _lock.unlock();
}

int RequestQueues::dequeueCPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(_queue_cpu, client_id, request, resolve);
}

int RequestQueues::dequeueGPU(clid_t &client_id,
                              json_t& request,
                              rply_t& resolve)
{
    return dequeue(_queue_gpu, client_id, request, resolve);
}

int RequestQueues::dequeue(std::deque<rqst_t> &queue,
                           clid_t &client_id,
                           json_t &request,
                           rply_t &resolve)
{
    _lock.lock();
    // TODO we forgot to remove the executed request ?? DONE ?
    auto it = std::find_if(queue.begin(), queue.end(), [&](rqst_t &req) { return req->isReady(); });
    if (it != queue.end()) {
        client_id = (*it)->getClientId();
        request = (*it)->getRequest();
        resolve = (*it)->getResolve();
        queue.erase(it); // << dequeue
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
        log() << "[Debug] new request received from client " << id << ": " << method << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

}}
