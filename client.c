#include "common.h"

int client_debug = 0;

int main(int argc, char *argv[]) {
    const char* server_name = "localhost";
    const int server_port = 8877;

    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s [message]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    // creates binary representation of server name
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(server_port);
    // htons: port in network order format

    // open socket
    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "could not create socket\n");
        return EXIT_FAILURE;
    }
    // connect the client socket to server socket
    if (connect(sock,(struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        fprintf(stderr,"connection with the server failed...\n");
        return EXIT_FAILURE;
    }
    // send data
    int len = send(sock, argv[1], strlen(argv[1]),0);
    // close the socket
    close(sock);
    return 0;
}
