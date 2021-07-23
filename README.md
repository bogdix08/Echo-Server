Client Server App

To acheive the requested functionality I've implemented two separate processes, a client and a server which communicate by TCP sockets.

Client only connects to the server and send the message received as argument. If connect fails client prints an error.

On the server side I've used a timerfd and epoll functionality to acheive the closing of the listening thread after 10 seconds.
In a separate thread the server listens for connections, accept them and stores the received message in a global queue. If the timer expired 
the thread ends. Main task is waiting for thread to join back and prints everything from the queue, than sleeps for 5 seconds.

Known issues: Even when sleeping, the server is not rejecting connections even if I've used listen with backlog 0 after thread joins...
so this request is not really working as expected: If the server is not present will report an error.
