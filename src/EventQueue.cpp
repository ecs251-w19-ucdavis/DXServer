//
// Created by Qi Wu on 2019-02-23.
//

#include "EventQueue.h"
#include <vector>

namespace v3d {
    namespace dx {
        Event::Event(int ClientId, int type, v3d::JsonValue request, response_t resolve)
        : _ClientId(ClientId), _type(type), _request(request), _resolve(resolve)
        {}
        void EventQueue::newRequest(int ClientId, int type, v3d::JsonValue request, response_t resolve)
        {
            Event newEvent(ClientId, type, request, resolve);
            std::string method = request.get("method", "").toString();
            switch(type) 
            {
            case 0: // a call
                Events.push_back(newEvent);
                if(method == "queryDatabase")
                    emit handleQueryDatabase(ClientId, type, request, resolve);
                else if(method == "openProject")
                    emit handleOpenProject(ClientId, type, request, resolve);
                else if(method == "closeProject")
                    emit handleCloseProject(ClientId, type, request, resolve);
                else if(method == "getScene")
                    emit handleGetScene(ClientId, type, request, resolve);
            case 1: // a notification
                auto it = std::find_if(Events.begin(), Events.end(), 
                                        [&ClientId, &type](Event& evnt){return (evnt.GetClient() == ClientId && evnt.GetType() == type);});
                if(it != Events.end()) // if there is a previous notification, replace it with the new one
                    *it = newEvent;
                else
                    Events.push_back(newEvent);
                emit handleRequestFrame(ClientId, type, request, resolve);
            }
        }
        
    }
}