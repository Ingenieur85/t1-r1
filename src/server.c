#include "define.h"


int main() {

/*
int argc, char *argv[]
    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        printf("Example: %s eth0\n", argv[0]);
        return 1;
    }
*/
    // Deals with file directories
    char server_files[2048];
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(server_files, sizeof(server_files), "%s/%s", cwd, SERVER_DIR);
        printf("\nServer files directory: %s\n", server_files);
    } else {
        perror("getcwd");
        return 1;
    }


    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);
    printf("Starting server on interface: %s\n", INTERFACE);

    //unsigned char buffer[BUFFER_SIZE];

    while (1) {
        packet pkt;

        if (receive_packet(socket_fd, &pkt)) {

            print_packet(pkt);

            
            FILE *fp = fopen("output.bin", "wb");
            if (fp == NULL) {
                perror("Failed to open file for writing");
                return 1;
            }

              
        }

        free_packet(&pkt);
    }

    close(socket_fd);
    return 0;
}
