# Note for Week 3

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

## Commits:
**Qi** 
* [DEBUG: re-enable the communication between the web-server and the redner-side-communicator by refactoring the position of `QApplication`](https://github.com/ecs251-w19-ucdavis/DXServer/commit/4ce24bd13b5a1df396b46c86d02a1c97c05dcfba)

* [improve abstraction by hiding some framebuffer operations inside the `SceneHandler`](https://github.com/ecs251-w19-ucdavis/DXServer/commit/abf2f716431485f9d0cf093a34bdf0c86920a6fb)

* [experiment a way to emit signals from the communicator and making it being received by a different component](https://github.com/ecs251-w19-ucdavis/DXServer/commit/bf396901b6d1c68908105a999bf293a0daace7bb)

* [add CMake code to dynamically generate the database.json file](https://github.com/ecs251-w19-ucdavis/DXServer/commit/bf396901b6d1c68908105a999bf293a0daace7bb)

**Yiran** 
* [implemented the initial design of an EventQueue (RequestQueue)](https://github.com/ecs251-w19-ucdavis/DXServer/commit/2a052398b74509f643cfbf3b977d1f994dfb0826)

* [added eventqueue to connection](https://github.com/ecs251-w19-ucdavis/DXServer/commit/2437e50c48fff1e786d621acb44970eeba02955e)

**Wenxi** 
* [added a map to store clients id and a map to store engines which every engine is associated with a specific client id](https://github.com/ecs251-w19-ucdavis/DXServer/commit/a2684a8eebd5504f854f2dcc195460b11cddf9de)

## Topic Discussed

#### RequestQueue (RQ) System Design

**Qi**: There are 6 possible requests in our current system: 

|Request|RequestType|Meaning|
|:-:|:-:|---|
|`openProject`|notification|Creates a rendering worker if necessary, opens a dataset from the disk and passes the data into the GPU.|
|`closeProject`|notification|Cleans the current OpenGL scene, stops rendering and delete the data if necessary.|
|`requestFrame`|notification|Renders an image using the given scene parameter.|
|`getScene`|call|Queries the current scene parameter.|
|`queryDatabase`|call|Reads the meta-data information for all the data stored on the server.|
|`disconnect`|notification|Closes the client’s connection and cause the client to be deleted.|

**Qi**: We distinguish these requests with two types (RequestType, RT) `call` and `notification`. A call can be appended to RQ, while a notification should override existing notifications in the queue with the same notification type and sent by the same client. See the example below:

| client - before | client - after |
|---|---|
|c1 - op|c1-op|
|c2 - op|c2-op|
|c1 - op|c1-rf|
|c1 - rf|c1-gs|
|c1 - gs|c1-gs|
|c1 - rf|c1-gs|
|c1 - gs|c1-gs|
|c1 - rf||
|c1 - gs||
|c1 - rf||
|c1 - gs||

**Yiran**: This **compression** is actually happening during the enqueue process. If RT is `notification`, RQ will check if there is an existing same Request and overwrite if so. 

**Yiran**: We define a `Request` in the `RequestQueue` (RQ) as a structure

```c
struct Request {
	int client_id;
	int request_type;
	JsonValue request;
	std::function<void(JsonValue)> resolve;
};
```
Here RequestType has two values: 
* 0 for **`call`** 
* 1 for **`notification`** 

Then the requests are aggregated into a queue `RequestQueue`. Currently in our program, the web communicator (WC) will emit a signal after receiving a request, which will be received by RQ’s slot `AddNewRequest`. This function will then create a new `Request`, perform the checking, push the `Request` to the queue, and emit a signal specified by `Request` to `RequestHandler` (RH).

**Yiran**: RQ should perform queue functionalities: instead of emitting signals immediately when receiving a request (just for testing), and instead of separating RQ and RH as distinct pipelines, RQ should integrate Request enqueue, dequeue and handling all together. This design makes more sense when we are implementing scheduling in future, since the scheduler will rearrange the requests’ sequence directly in the queue.


**Problems**:  
* There might be dependencies between requests. We need a mechanism to maintain this dependencies. For example a OP should happen before a RF, GS or CP. 

* **Qi**: One way to solve this problem is to implement add an atomic request counter for each client. When a new request is being pushed into the queue, the request will remember an expected request count. If the request’s expected count equals to the current request count of that particular client, we say this request is ready for execution and it can be executed whenever the handler will be available. Once the request has been executed, we increment the request counter by one.

* Some requests can be executed in parallel. For example loading a data can happen in parallel with rendering a frame for a different client. Therefore it requires us to maintain two handlers running on two threads. One thread focuses on GPU rendering while the other focuses on all the CPU-based tasks. 
	
* **Qi**’s explanation for above problem: For example, OP is an operation consists of loading the data and initializing it in OpenGL. So it makes sense to internally divide the request into a `loadData` sub-request and a `initGL` sub-request. Those two requests can run on different threads but there is a dependency between them: for one client, `loadData` should happen before `initGL`.

* **Yiran**: We can implement scheduling only for GPU related requests, and run CPU related requests concurrently.

**Qi**: We need to define invariants when scheduling requests. 
1. For one client, the order of requests to **execute** should be the same as the requests’ receiving order after compression.
2. There is no ordering guarantees between requests for different clients.

#### Communication between the Web-Server & the Render-Communicator

**Wenxi**: In order to realize the communication correctly between multiple Web-Server and Render-Communicator, first, we create a map to store a socket associated with a client id, and create another map to store the client id associated with an engine, which includes requests of the a client. Then, every time there is a connection with a new client, we store the client in the map and create a new engine with the client. If the client disconnects, we delete the client id in the map and simultaneously delete the engine associated with the client. 

**Qi**: We realized that we should maintain a list of engines and list of sockets globally.

**Problem**: “Duplicated” Connection each time when we start a connection from a web-client, we will first saw a connection being established, and then a second connection will be established again. We are not sure what is happening.

**TODO**: We already designed a structure that can realize the connection between Web-server and Render-Communicator but still need to modify the connection section since every connection always appear twice.

