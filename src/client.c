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
    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);
    printf("Starting client on interface: %s\n", INTERFACE);

    
    packet pkt;

    const unsigned char test = 'A';
    uint8_t seq = 30;
    seq = ((seq + 1) & 0b00011111);
    for (int i = 0; i < 4; i++) {
        build_packet(&pkt, strlen((const char *)error_message[i]), seq, ERROR, error_message[2]);
    }

    //build_packet(&pkt, 0, seq, NACK, NULL);
    
    print_packet(pkt);
    send_packet(socket_fd, &pkt);


    const char *file_path = "/home/fds/redes1/t1/server_files/eneida.txt";
    read_and_print_file(file_path);




    free_packet(&pkt);
    close(socket_fd);
    return 0;
}
