//
// Created by Qi Wu on 2019-02-23.
//

#include "RequestHandler.h"

#include "Util/Log.h"

//void v3d::dx::handle_OpenProjectRequested(std::string projFileName, int clientId)
//{
//
//}

void v3d::dx::RequestHandler::handleNewRequest(int clientId, int type, v3d::JsonValue request,
                                               const std::function<void(v3d::JsonValue)>& resolve)
{
    log() << "[debug] got new request" << std::endl;
}