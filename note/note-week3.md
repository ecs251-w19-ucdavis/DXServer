# Note for Week 3

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

## Topic Discussed

#### RequestQueue (RQ) System Design

* **Qi**: There are 6 possible requests in our current system: 

|Request|RequestType|Meaning|
|:-:|:-:|---|
|`openProject`|notification|Creates a rendering worker if necessary, opens a dataset from the disk and passes the data into the GPU.|
|`closeProject`|notification|Cleans the current OpenGL scene, stops rendering and delete the data if necessary.|
|`requestFrame`|notification|Renders an image using the given scene parameter.|
|`getScene`|call|Queries the current scene parameter.|
|`queryDatabase`|call|Reads the meta-data information for all the data stored on the server.|
|`disconnect`|notification|Closes the client’s connection and cause the client to be deleted.|

* **Qi**: We distinguish these requests with two types (RequestType, RT) `call` and `notification`. A call can be appended to RQ, while a notification should override existing notifications in the queue with the same notification type and sent by the same client. See the example below:

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

* **Yiran**: This **compression** is actually happening during the enqueue process. If RT is `notification`, RQ will check if there is an existing same Request and overwrite if so. 

* **Yiran**: We define a `Request` in the `RequestQueue` (RQ) as a structure
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

* **Yiran**: RQ should perform queue functionalities: instead of emitting signals immediately when receiving a request (just for testing), and instead of separating RQ and RH as distinct pipelines, RQ should integrate Request enqueue, dequeue and handling all together. This design makes more sense when trying to 


* **Problems**:  
	* There might be dependencies between requests. We need a mechanism to maintain this dependencies. For example a OP should happen before a RF, GS or CP. 

	* **Qi**: One way to solve this problem is to implement add an atomic request counter for each client. When a new request is being pushed into the queue, the request will remember an expected request count. If the request’s expected count equals to the current request count of that particular client, we say this request is ready for execution and it can be executed whenever the handler will be available. Once the request has been executed, we increment the request counter by one.

	* Some requests can be executed in parallel. For example loading a data can happen in parallel with rendering a frame for a different client. Therefore it requires us to main two handlers running on two threads. One thread focuses on GPU rendering while the other focuses on all the CPU-based tasks. 
	
	* **Qi**: OP is an operation consists of loading the data and initializing it in OpenGL. So it makes sense to internally divide the request into a `loadData` sub-request and a `initGL` sub-request. Those two requests can run on different threads but there is a dependency between them: for one client, `loadData` should happen before `initGL`.

* **Qi**: We need to define invariants when scheduling requests. 
1. For one client, the order of requests to **finish** should be the same as the requests’ receiving order after compression.
2. There is no ordering guarantees between requests for different clients.

#### Communication between the Web-Server & the Render-Communicator

* **Wenxi**: In order to realize the communication correctly between multiple Web-Server and Render-Communicator, first, we create a map to store a socket associated with a client id, and create another map to store the client id associated with an engine, which includes requests of the a client. Then, every time there is a connection with a new client, we store the client in the map and create a new engine with the client. If the client disconnects, we delete the client id in the map and simultaneously delete the engine associated with the client. 

* **Qi**: 
