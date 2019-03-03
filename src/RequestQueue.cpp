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

///////////////////////////////////////////////////////////////////////////////

static v3d::dx::api::queues_t global_queue;

namespace v3d { namespace dx {
namespace queues {

void create()
{
    global_queue = std::make_shared<RequestQueues>();
}

api::queues_t ref()
{
    return global_queue;
}

}
}}

///////////////////////////////////////////////////////////////////////////////

v3d::dx::Request::Request(int ClientId, int type, v3d::JsonValue request, api::response_t resolve)
    : _client_id(ClientId)
    , _type(type)
    , _request(std::move(request))
    , _resolve(std::move(resolve))
{}

void v3d::dx::RequestQueues::EnqueueRequest(int client_id, int type, v3d::JsonValue request, api::response_t resolve)
{
    Request newRequest(client_id, type, request, std::move(resolve));
    std::string method = request.get("method", "").toString();
    std::cout << "new request received from client " << client_id << ": " << method << std::endl;
    switch (type) {
        case 0: { // a call
            QueueCPU.push_back(newRequest);
            // if(method == "queryDatabase")
            //     emit handleQueryDatabase(ClientId, type, request, resolve);
            // else if(method == "openProject")
            //     emit handleOpenProject(ClientId, type, request, resolve);
            // else if(method == "closeProject")
            //     emit handleCloseProject(ClientId, type, request, resolve);
            // else if(method == "getScene")
            //     emit handleGetScene(ClientId, type, request, resolve);
            break;
        }
        case 1: { // a notification
            auto it = std::find_if(QueueCPU.begin(), QueueCPU.end(), [&client_id, &type](Request &req)
            {
                return (req.getClientId() == client_id && req.getRequestType() == type);
            });
            if (it != QueueCPU.end()) // if there is a previous notification, replace it with the new one
                *it = newRequest;
            else
                QueueCPU.push_back(newRequest);
            // emit handleRequestFrame(ClientId, type, request, resolve);
            break;
        }
        default: throw std::runtime_error("[Error] unknown request type");
    }
}
        

