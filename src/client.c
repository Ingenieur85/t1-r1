#include "define.h"

int main() {
    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);
    packet pkt;
    //unsigned char buffer[BUFFER_SIZE];
    //strcpy((char *)buffer, MESSAGE);

#if 0
    printf("TAM errormessage[2]: %ld \n", strlen((const char *)error_message[2]));

    build_packet(&pkt, strlen((const char *)error_message[2]), 0b00000, 0b11111, error_message[2]);
    print_packet(pkt);

    //printf("Resultado checkcrc: %d \n", check_crc8(&pkt));
    // Send message to server
    send_packet(socket_fd, &pkt);
#endif

    const unsigned char test = 'A';
    //build_packet(&pkt, strlen((const char *)error_message[2]), 0b00000, ERROR, error_message[2]);
    uint8_t seq = 30;
    seq = ((seq + 1) & 0b00011111);
    build_packet(&pkt, 1, seq, NACK, &test);
/*  
    pkt.init = INIT_MARKER;
    pkt.size_seq_type[0] = 0b11111100;
    pkt.size_seq_type[1] = 0b00011111;
    pkt.data = (unsigned char *)"Hellooooooooooooooooooo darkness my old FRIEND!@#$";
    pkt.crc = 0b10000001;
*/
    
    print_packet(pkt);
    send_packet(socket_fd, &pkt);



    free_packet(&pkt);
    close(socket_fd);
    return 0;
}
