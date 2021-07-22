#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include "queue.h"
#include <pthread.h>

#define MAXEVENTS 64

// global sockets
int sock, tfd, epfd;
struct Queue *q;

void * listenThread(void *arg)
{
    struct sockaddr_in client_address;
    int client_address_len = 0;
    struct epoll_event events[ MAXEVENTS ];

    /* Buffer where events are returned */
    //events = calloc (MAXEVENTS, sizeof(struct epoll_event));
    printf("start waiting for connections\n");
    /* The event loop */
    while (1)
    {
        int n, i;

        n = epoll_wait (epfd, events, MAXEVENTS, -1);
printf("got something, n = %d\n", n);
        for (i = 0; i < n; i++) {
printf("got something, fd = %d\n", events[i].data.fd);
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                    /* An error has occured on this fd, or the socket is not
                       ready for reading (why were we notified then?) */
                    fprintf (stdout, "epoll error\n");
                    close (events[i].data.fd);
                    continue;
                }
            else if (sock == events[i].data.fd) {
                char buffer[500];
                // read content into buffer from an incoming client
                int len = recvfrom(sock, buffer, sizeof(buffer), 0,
                           (struct sockaddr *)&client_address,
                           &client_address_len);
                if(len <= 0) {
                    printf("null message\n");
                    break;
                }
                // inet_ntoa prints user friendly representation of the
                // ip address
                buffer[len] = '\0';
                printf("received: '%s' from client %s\n", buffer,
                       inet_ntoa(client_address.sin_addr));
                enQueue(q, buffer);
            }
            else if (tfd == events[i].data.fd) {
                printf("timer expired\n");
                 char buffer[500];
                // read content into buffer from an incoming client
                int len = recvfrom(sock, buffer, sizeof(buffer), 0,
                           (struct sockaddr *)&client_address,
                           &client_address_len);
                if(len <= 0) {
                    printf("null message\n");
                    break;
                }
                return;
            }
            else {
                printf("undefined behavior\n");
            }
        }
    }
}


int main(int argc, char *argv[]) {
    // port to start the server on
    int SERVER_PORT = 8877;
    struct epoll_event ev,ev2;
    struct itimerspec ts;
    int timer_interval = 10; // timer fires after 10sec
    pthread_t  tid;
    // socket address used for the server
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    // htons: host to network short: transforms a value in host byte
    // ordering format to a short value in network byte ordering format
    server_address.sin_port = htons(SERVER_PORT);

    // htons: host to network long: same as htons but to long
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // create a UDP socket, creation returns -1 on failure
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("could not create socket\n");
        return 1;
    }

    // bind it to listen to the incoming connections on the created server
    // address, will return -1 on error
    if ((bind(sock, (struct sockaddr *)&server_address,
              sizeof(server_address))) < 0) {
        printf("could not bind socket\n");
        return 1;
    }

    tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd == -1) {
        printf("timerfd_create() failed: errno=%d\n", errno);
        return EXIT_FAILURE;
    }

    printf("created timerfd %d\n", tfd);

    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = timer_interval;
    ts.it_value.tv_nsec = 0;
    // socket address used to store client address

    epfd = epoll_create(2);
    if (epfd == -1) {
        printf("epoll_create() failed: errno=%d\n", errno);
        close(tfd);
        return EXIT_FAILURE;
    }
    printf("created epollfd %d\n", epfd);

    ev.events = EPOLLIN;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &ev) == -1) {
        printf("epoll_ctl(ADD) failed: errno=%d\n", errno);
        close(epfd);
        close(tfd);
        return EXIT_FAILURE;
    }
    ev2.events = EPOLLIN;
 
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev2) == -1) {
        printf("epoll_ctl(ADD) failed: errno=%d\n", errno);
        close(epfd);
        close(tfd);
        return EXIT_FAILURE;
    }


    q = createQueue();
    while(1) {
        if(pthread_create(&tid, NULL, listenThread,NULL) != 0 )
               printf("Failed to create thread\n");

        if (timerfd_settime(tfd, 0, &ts, NULL) < 0) {
            printf("timerfd_settime() failed: errno=%d\n", errno);
            close(tfd);
            return EXIT_FAILURE;
        }
        printf("set timerfd time=\n");

        pthread_join(tid, NULL);
        printf("done join\n");
        struct QNode *temp;
        //print all messages
        while((temp = deQueue(q)) != NULL){
            printf("%s\n", temp->message);
        }
        //sleep
        sleep(5);
    }

    return 0;
}
