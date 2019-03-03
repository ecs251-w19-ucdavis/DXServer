//
// Created by Qi Wu on 2019-02-23.
//

#include "RequestQueue.h"
#include <queue>

namespace v3d {
    namespace dx {
        Request::Request(int ClientId, int type, v3d::JsonValue request, response_t resolve)
        : _ClientId(ClientId), _type(type), _request(request), _resolve(resolve)
        {}
        void RequestQueues::EnqueueRequest(int ClientId, int type, v3d::JsonValue request, response_t resolve)
        {
            Request newRequest(ClientId, type, request, resolve);
            std::string method = request.get("method", "").toString();
            std::cout << "new request received from client " << ClientId << ": " << method << std::endl;
            switch(type) 
            {
            case 0: // a call
                QueueCPU.push_back(newRequest);
                // if(method == "queryDatabase")
                //     emit handleQueryDatabase(ClientId, type, request, resolve);
                // else if(method == "openProject")
                //     emit handleOpenProject(ClientId, type, request, resolve);
                // else if(method == "closeProject")
                //     emit handleCloseProject(ClientId, type, request, resolve);
                // else if(method == "getScene")
                //     emit handleGetScene(ClientId, type, request, resolve);
            case 1: // a notification
                auto it = std::find_if(QueueCPU.begin(), QueueCPU.end(), 
                                        [&ClientId, &type](Request& req){return (req.GetClient() == ClientId && req.GetType() == type);});
                if(it != QueueCPU.end()) // if there is a previous notification, replace it with the new one
                    *it = newRequest;
                else
                    QueueCPU.push_back(newRequest);
                // emit handleRequestFrame(ClientId, type, request, resolve);
            }
        }
        
    }
}