#include "common.h"

int client_debug = 0;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf (stderr, "Usage: %s [message]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(SERVER_PORT);

    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "could not create socket\n");
        return EXIT_FAILURE;
    }

    if (connect(sock,(struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        fprintf(stderr,"connection with the server failed...\n");
        return EXIT_FAILURE;
    }

    int len = send(sock, argv[1], strlen(argv[1]),0);
    close(sock);
    return 0;
}
