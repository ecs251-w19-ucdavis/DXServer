# Note for Week 2

[link to trello](https://trello.com/invite/daxian2/0d1bdd82285db70339a7885d70934329)

### What did we do last week

In general, 

##### Qi
* [created a json reader for loading the data](https://github.com/ecs251-w19-ucdavis/DXServer/commit/88fb75e9d0a6af89007a3f279a1d785dcb5e80f0)
* [improved the json reader for parsing more rendering parameters](https://github.com/ecs251-w19-ucdavis/DXServer/commit/42149cc9a3d79be5e5a1d0f873db83b19e8e42f2)
* [WIP added a tetrahedral grid loader](https://github.com/ecs251-w19-ucdavis/DXServer/commit/d9a2472730e4e34a528f04542dbf9ed52f2495bf)

##### Yiran
* read and studied the rendering code
* [set up localized file loader and renderer](https://github.com/ecs251-w19-ucdavis/DXServer/commit/79f3a25c71e8467291f9dd5b884956c5c4d5eac5#diff-25d902c24283ab8cfbac54dfa101ad31)
* [set up the basic client structure](https://github.com/ecs251-w19-ucdavis/DXServer/commit/2ae6274ebc3c124b329f5513852e7b46523cf3a7#diff-25d902c24283ab8cfbac54dfa101ad31)

##### Wenxi
* read code and study Node.js, JavaScript
* [Figure an approach to sent each client's request to a specific VoxEngine](https://github.com/ecs251-w19-ucdavis/DXServer/commit/2c0712a842529a26fa8b906aa1aab3930b8ab6c2)
* [Try to read each client's request associated with it's id so that we can create a specific VocEngine for each client's request according to different id](https://github.com/ecs251-w19-ucdavis/DXServer/commit/36546c8711a2c594b0f384712db26328656960bc)


### What are we going to do this week
##### Qi
* together with Yiran, we will be designing system for handling dynamic requests from simulated clients
* together with Wenxi, we will be setting up the render-side communicator so that our render-side system can receive requests from many clients.

##### Yiran
* create and maintain different requests from clients
* send results back to web server - achieve connections
* figure out what scheduling techniques can be applied

##### Wenxi
* still try to figure out an efficient approach to sent each client's request to a specific VoxEngine
* send webserver requests to rendering system

### Issues
<<<<<<< HEAD
=======
* We didn't use the actual thread for abstraction. Might need to switch to threads if there are more than one GPU.
>>>>>>> 8c8ad1f456b90fd147f84afbb87319a2b4d66fa2
