#include "define.h"


int main() {
    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);

    //unsigned char buffer[BUFFER_SIZE];
    //strcpy((char *)buffer, MESSAGE);

    printf("TAM errormessage[2]: %ld \n", strlen((const char *)error_message[2]));

    packet pkt;

    build_packet(&pkt, strlen((const char *)error_message[2]), 0b00000, 0b11111, error_message[2]);


/*
    pkt.init = INIT_MARKER;
    pkt.size_seq_type[0] = 0b11111100;
    pkt.size_seq_type[1] = 0b00011111;
    pkt.data = (unsigned char *)"Hellooooooooooooooooooo darkness my old FRIEND!@#$";
    pkt.crc = 0b10000001;
*/
    print_packet(pkt);

    printf("Resultado checkcrc: %d \n", check_crc8(&pkt));
    // Send message to server
    int length = send(socket_fd, &pkt, sizeof(pkt), 0);
    if (length == -1) {
        perror("Error sending data");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    free_packet(&pkt);

    close(socket_fd);
    return 0;
}
