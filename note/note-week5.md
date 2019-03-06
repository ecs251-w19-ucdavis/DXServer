# Current Problems

* Handling ``closeProject`` opposite operations to openProject

* Implementing a thread pool API for CPU requests

* Current request handler thread: busy waiting when no requests in queue
  
* Find out ways to close client after some timeout, take care of cancelling ``closeClient`` if the client connects during the timeout
  https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for
  
* Fix a minor problem: have to drag the mouse to send the "real" scene (after selection, only an empty json value will be sent)
