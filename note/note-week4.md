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

**Qi**: Configuring a fixed expectation for each request would be hard and not necessary. For now, we can find out 
