#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    const char* server_name = "localhost";
    const int server_port = 8877;

    if (argc != 2)
    {
      fprintf (stderr, "Usage: %s [message]\n", argv[0]);
      exit (EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    // creates binary representation of server name
    // and stores it as sin_addr
    // http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
    inet_pton(AF_INET, server_name, &server_address.sin_addr);

    // htons: port in network order format
    server_address.sin_port = htons(server_port);

    // open socket
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("could not create socket\n");
        return 1;
    }

    // send data
    int len =
        sendto(sock, argv[1], strlen(argv[1]), 0,
               (struct sockaddr*)&server_address, sizeof(server_address));
    if (len < 0) {
        printf("could not send\n");
    }
    // close the socket
    close(sock);
    return 0;
}
