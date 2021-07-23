#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
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
#define MAXCLIENTS 10
#define DEBUG_SRV_INFO(f_, ...) if (server_debug) { printf("%s:%d: " f_ , __func__, __LINE__, ##__VA_ARGS__); }
#define DEBUG_CLT_INFO(f_, ...) if (client_debug) { printf("%s:%d: " f_ , __func__, __LINE__, ##__VA_ARGS__); }



