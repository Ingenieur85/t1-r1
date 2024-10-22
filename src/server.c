#include "define.h"


int main() {
    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);

    //unsigned char buffer[BUFFER_SIZE];

    while (1) {
        packet pkt;

        if (receive_packet(socket_fd, &pkt)) {
            //TODO remove
            // Print the actual data in the packet (up to 'size')
            //for (int i = 0; i < get_packet_size(&pkt); i++) {
            //    printf("%02X ", pkt.data[i]);
            //}
            print_packet(pkt);
        }
        free_packet(&pkt);

#if 0        
        // Clear the buffer to avoid leftover data
        memset(buffer, 0, BUFFER_SIZE);

        // Receive messages (blocking call)
        int length = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0); // -1 to reserve space for null-termination
        if (length == -1) {
            perror("Error receiving data");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        // Ensure null-termination of the received message
        buffer[length] = '\0';

        // Print the received message
        printf("Server received message: %s\n", buffer);
        fwrite(buffer, 1, sizeof(buffer), stdout);
#endif
    }

    close(socket_fd);
    return 0;
}
