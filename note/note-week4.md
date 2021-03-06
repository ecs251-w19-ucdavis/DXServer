# Note for Week 4

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

## Commits
**Qi** 

[implemented a request counter](https://github.com/ecs251-w19-ucdavis/DXServer/commit/5a2b38a5f3fc1cfffe63c6ee37784162ab09a2f2)

[implemented a client queue](https://github.com/ecs251-w19-ucdavis/DXServer/commit/fe8ee4bd5a41206e1e3607b9b1fb371e2d09da2f)

[add CPU & GPU handlers, add function to access the global queue](https://github.com/ecs251-w19-ucdavis/DXServer/commit/3d1045d4ce70a433ead413aecb3693c1d519b38b)

[connect clients to render-server for the first time](https://github.com/ecs251-w19-ucdavis/DXServer/commit/433bf09fe79454b864b2d5ac24070e77915b3f50)

**Yiran**  

[splitted requests into subrequests, and pushed into two different queues](https://github.com/ecs251-w19-ucdavis/DXServer/commit/73ff6f3220f0618b6dd086434a9f8b1eb3e09b79)

**Wenxi**
* [Complete connection between client-webserver and render engine](https://github.com/ecs251-w19-ucdavis/DXServer/commit/0e7b612c31c07405040cd2f8bec6cf470b6cb293)
* [Add documentations to RequestsHandler that need to be dealt with in CPU](https://github.com/ecs251-w19-ucdavis/DXServer/commit/1ef85d8e8a8995c81792a34535c4b65191ec6c85)
* [Requests that need to be dealt with in GPU](https://github.com/ecs251-w19-ucdavis/DXServer/commit/c265a6efa2d4196c6593de3bad15cf31f627de80)

## Topic Discussed

### Request Counter Implementation

Besides two request queues (``QueueCPU`` and ``QueueGPU``), a list of clients should be maintained to keep the current request counter for each client.

**Yiran**: Exhaustively list all possible dependencies, assign a fixed expectation according to each request's name. For the atomic counter, we can iterate its values periodically, following the dependencies.

In one word, the "expected" counter value for each request is only related to the request name ("method").

**Qi**: Configuring a fixed expectation for each request would be hard and not necessary. For now, we can assume each customer is well-behaved and will make requests in correct orders. In this way, the only invariant we need to keep is that the ``RequestHandler`` processes each client's requests in the same order as he originally made. 

**Qi**: We don't need to set a fixed expectation value according to each request's name. Instead, upon enqueueing a request, we can check the client's largest expectation value currently in the queue, and set the enqueueing request's expectation to be that value + 1.

But this solution might need searching through the queue on every enqueue.

**Qi**'s solution: Besides keeping a current request counter for each client, we can also keep a ``next_request_id``, which will be incremented only when a request enqueues.


### Client-Webserver and Render Engine Connection

**Wenxi**: For each client, we create a new engine to connect the client to the render engine. That is to say, we have two maps, first is to store clients, since each socket represents a client, we store the client id as a key, and store the socket as the value; second map is to store the engine corresponds to the client, we store the client id as the key,  and store the engine as the value.
Each engine includes the requests of clients that we need to connect to the render engine.
