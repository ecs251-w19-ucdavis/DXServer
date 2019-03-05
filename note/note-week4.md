# Note for Week 4

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

## Commits
**Qi** 

**Yiran**

**Wenxi**

##Topic Discussed

###Request Counter Implementation

Besides two request queues (``QueueCPU`` and ``QueueGPU``), a list of clients should be maintained to keep the current request counter for each client.

**Yiran**: Exhaustively list all possible dependencies, assign a fixed expectation according to each request's name. For the atomic counter, we can iterate its values periodically, following the dependencies.

In one word, the "expected" counter value for each request is only related to the request name ("method").

**Qi**: Configuring a fixed expectation for each request would be hard and not necessary. For now, we can assume each customer is well-behaved and will make requests in correct orders. In this way, the only invariant we need to keep is that the ``Request Handler`` processes each client's requests in the same order as he originally made. 

**Qi**: We don't need to set a fixed expectation value according to each request's name. Instead, upon enqueueing a request, we can check the client's largest expectation value currently in the queue, and set the enqueueing request's expectation to be that value + 1.

But this solution might need searching through the queue on every enqueue.

**Qi**'s solution: Besides keeping a 


###Client-Webserver and Render Engine Connection

**Wenxi**: For each client, we create a new engine to connect the client to the render engine. That is to say, we have two maps, first is to store clients, since each socket represents a client, we store the client id as a key, and store the socket as the value; second map is to store the engine corresponds to the client, we store the client id as the key,  and store the engine as the value.
Each engine includes the requests of clients that we need to connect to the render engine.
