# Note for Week 3

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

## Topic Discussed
#### Request Queue System Design

* **Qi**: There are 6 possible requests in our current system: 

|request|type|meaning|
|:-:|:-:|---|
|`openProject`|notification|Creates a rendering worker if necessary, opens a dataset from the disk and passes the data into the GPU.|
|`closeProject`|notification|Cleans the current OpenGL scene, stops rendering and delete the data if necessary.|
|`requestFrame`|notification|Renders an image using the given scene parameter.|
|`getScene`|call|Queries the current scene parameter.|
|`queryDatabase`|call|Reads the meta-data information for all the data stored on the server.|
|`disconnect`|notification|Closes the client’s connection and cause the client to be deleted.|

* **Qi**: One important difference between a call and a notification is that, a notification can override existing notifications in the queue as long as they are of the same notification type and they are requested by the same client. This requires the `EventQueue` to pay extra attentions when deciding a scheduling. 

* **Yiran**: We define a `Request` in the `RequestQueue` (RQ) as a structure
```c
struct Request {
    int ClientID;
    int RequestType;
    JsonValue Request;
    std::function<void(JsonValue)> resolve;
};
```
