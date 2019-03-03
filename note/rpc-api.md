# RPC API Specification

There are 6 possible requests in our current system: 

|Request|RequestType|Meaning|
|:-:|:-:|---|
|`openProject`|notification|Creates a rendering worker if necessary, opens a dataset from the disk and passes the data into the GPU.|
|`closeProject`|notification|Cleans the current OpenGL scene, stops rendering and delete the data if necessary.|
|`requestFrame`|notification|Renders an image using the given scene parameter.|
|`getScene`|call|Queries the current scene parameter.|
|`queryDatabase`|call|Reads the meta-data information for all the data stored on the server.|
|`disconnect`|notification|Closes the client’s connection and cause the client to be deleted.|

