#include "define.h"


int main() {
    // Create the raw socket using the loopback interface (lo)
    int raw_socket = cria_raw_socket(INTERFACE);

    unsigned char buffer[BUFFER_SIZE];

    while (1) {
        // Clear the buffer to avoid leftover data
        memset(buffer, 0, BUFFER_SIZE);

        // Receive messages (blocking call)
        int length = recv(raw_socket, buffer, BUFFER_SIZE - 1, 0); // -1 to reserve space for null-termination
        if (length == -1) {
            perror("Error receiving data");
            close(raw_socket);
            exit(EXIT_FAILURE);
        }

        // Ensure null-termination of the received message
        buffer[length] = '\0';

        // Print the received message
        printf("Server received message: %s\n", buffer);

        fwrite(buffer, 1, sizeof(buffer), stdout);
    }

    close(raw_socket);
    return 0;
}
