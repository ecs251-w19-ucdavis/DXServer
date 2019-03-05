//
// Created by Qi Wu on 2019-03-02.
//

#pragma once
#ifndef DXSERVER_GPUTASKHANDLER_H
#define DXSERVER_GPUTASKHANDLER_H


namespace v3d { namespace dx {

/**
 * This class suppose to handle requests from the event queue. This class will open and run in a different thread.
 */
class GPUTaskHandler {
public:
    /** This 
     *       
     */
    explicit GPUTaskHandler();
    void processNextRequest();

private:
    void initOpenGL();
    void requestFrame();
    void getScene();
    void queryDatabase();

private:

};

}}

#endif //DXSERVER_GPUTASKHANDLER_
