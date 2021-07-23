#include "common.h"

// global sockets
static int  timer_fd, epfd;
static struct Queue *q;
int server_debug = 1;

int addfd( int epollfd, int fd )
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) {
        fprintf(stderr, "epoll_ctl(ADD) failed: errno=%d\n", errno);
        return EXIT_FAILURE;
    }
    return 0;
}

void * listenThread(void *arg)
{
    struct sockaddr_in client_address;
    int SERVER_PORT = 8877;
    int client_address_len = 0;
    struct epoll_event events[ MAXEVENTS ] = {0};
    int listen_fd, data_fd;
    DEBUG_SRV_INFO("start waiting for connections on timer_fd %d epfd %d\n", timer_fd, epfd );
     // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    // htons: host to network short: transforms a value in host byte
    // ordering format to a short value in network byte ordering format
    server_address.sin_port = htons(SERVER_PORT);

    // htons: host to network long: same as htons but to long
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // create a TCP socket, creation returns -1 on failure
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return (void*)(uint64_t)errno;
    }
    DEBUG_SRV_INFO("listen_fd %d\n", listen_fd );
 
    // bind it to listen to the incoming connections on the created server
    // address, will return -1 on error
    if ((bind(listen_fd, (struct sockaddr *)&server_address,
              sizeof(server_address))) < 0) {
            close(listen_fd);
   return (void*)(uint64_t)errno;
    }


    if(listen(listen_fd, MAXCLIENTS) < 0) {
  return (void*)(uint64_t)errno;
    }

    if (addfd(epfd, listen_fd) == EXIT_FAILURE) {
            close(listen_fd);
            close(epfd);
 return (void*)(uint64_t)errno;
    }

  /* The event loop */
    while (1)
    {
        int n, i, len;

        n = epoll_wait (epfd, events, MAXEVENTS, -1);
       if(n==-1) {
         fprintf(stderr, "epoll error\n");
         return (void*)(uint64_t)errno;
       } else {
            for (i = 0; i < n; i++) {
                if (listen_fd == events[i].data.fd && events[i].events & EPOLLIN) {

                    data_fd = accept( listen_fd, ( struct sockaddr* )&client_address, &client_address_len);
                                 //Register the new connection socket as a readable event
                    DEBUG_SRV_INFO("New connection on data_fd %d\n", data_fd);
                    addfd( epfd, data_fd );
                }
                else if (timer_fd == events[i].data.fd && events[i].events & EPOLLIN) {
                    uint64_t exp;
                    DEBUG_SRV_INFO("timer expired event\n");
                   read(timer_fd, &exp, sizeof(exp));
                    return (void*)EAGAIN;
                }
                else if ( events[i].events & EPOLLIN ) {
 
                    char buffer[500];
                    // read content into buffer from an incoming client
                    len = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                    if(len < 0) {
                        fprintf(stderr, "error on receive message\n");
                        close( events[i].data.fd);
                        break;
                    }
                    // inet_ntoa prints user friendly representation of the
                    // ip address
                    buffer[len] = '\0';
                   DEBUG_SRV_INFO("received: %s\n", buffer);
                    enQueue(q, buffer);
                    close(events[i].data.fd);
                }
            }
        }
    }

return (void*)EINVAL;
}


int main(int argc, char *argv[]) {
    // port to start the server on
    struct itimerspec ts;
    int timer_interval = 10; // timer fires after 10sec
    pthread_t  tid;


    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1) {
        fprintf(stderr, "timerfd_create() failed: errno=%d\n", errno);
        return EXIT_FAILURE;
    }

    DEBUG_SRV_INFO("created timerfd %d\n", timer_fd);
   // socket address used to store client address
    ts.it_interval.tv_sec = 15;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = timer_interval;
    ts.it_value.tv_nsec = 0;
 
    if (timerfd_settime(timer_fd, 0, &ts, NULL) < 0) {
        fprintf(stderr, "timerfd_settime() failed: errno=%d\n", errno);
        close(timer_fd);
        close(epfd);
        return EXIT_FAILURE;
    }


    epfd = epoll_create(2);
    if (epfd == -1) {
        fprintf(stderr, "epoll_create() failed: errno=%d\n", errno);
             close(timer_fd);
            close(epfd);
        return EXIT_FAILURE;
    }
    DEBUG_SRV_INFO("created epollfd %d\n", epfd);


    if (addfd(epfd, timer_fd) == EXIT_FAILURE) {
             close(timer_fd);
            close(epfd);
        return EXIT_FAILURE;
    }

     q = createQueue();
    while(1) {
        DEBUG_SRV_INFO("Creating listening thread\n");
        if(pthread_create(&tid, NULL, listenThread,NULL) != 0 )
               fprintf(stderr, "Failed to create thread\n");

        pthread_join(tid, NULL);
        DEBUG_SRV_INFO("done join\n");
        struct QNode *temp;
        //print all messages
        while((temp = deQueue(q)) != NULL){
            printf("%s\n", temp->message);
        }
        //sleep
        DEBUG_SRV_INFO("sleeping 5 sec\n");
        sleep(5);
    }

    return 0;
}
