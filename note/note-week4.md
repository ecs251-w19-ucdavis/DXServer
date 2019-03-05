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

**Qi**: Configuring a fixed expectation for each request would be hard and not necessary. For now, we can assume each customer is well-behaved and will make requests in correct orders. In this way, the only invariant we need to keep is that the ``RequestHandler`` processes each client's requests in the same order as he originally made. 

**Qi**: We don't need to set a fixed expectation value according to each request's name. Instead, upon enqueueing a request, we can check the client's largest expectation value currently in the queue, and set the enqueueing request's expectation to be that value + 1.

But this solution might need searching through the queue on every enqueue.

**Qi**'s solution: Besides keeping a current request counter for each client, we can also keep a ``next_request_id``, which will be incremented only when a request enqueues.


