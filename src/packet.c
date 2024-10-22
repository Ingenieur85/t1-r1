#include "define.h"

void build_packet(packet *pkt, uint8_t size, uint8_t seq, uint8_t type, const unsigned char *data) { 
    pkt->init = INIT_MARKER;
    pack_fields(pkt, size, seq, type);

    if (size == 0 || data == NULL) {
        pkt->data = NULL;
        
    } else {
        pkt->data = (unsigned char *)malloc(size);
        if (pkt->data == NULL) {
            perror("Packet->data malloc failed.");
            return;
        }
        memcpy(pkt->data, data, size);
    }
/*    
    //TODO remove
    // Print the actual data in the packet (up to 'size')
    for (int i = 0; i < size; i++) {
        printf("%02X ", pkt->data[i]);
    }
*/
    pkt->crc = calculate_crc8(pkt);
    return;
}

int send_packet(int socket_fd, packet *pkt) {
    // Calculate the total size of the packet to send
    uint8_t size, seq, type;
    unpack_fields(pkt, &size, &seq, &type);
    
    int total_length = sizeof(pkt->init) + sizeof(pkt->size_seq_type) + size + sizeof(pkt->crc);
    printf("Total length: %d\n", total_length);
    
    // Allocate a buffer to hold the entire packet
    unsigned char *buffer = (unsigned char *)malloc(total_length);
    if (buffer == NULL) {
        perror("Buffer malloc failed.");
        return 0;
    }
    
    // Copy fields into the buffer
    buffer[0] = pkt->init;
    buffer[1] = pkt->size_seq_type[0];
    buffer[2] = pkt->size_seq_type[1];

    // Copy data into the buffer if available
    if (size > 0 && pkt->data != NULL) {
        memcpy(&buffer[3], pkt->data, size);
    }

    // Copy the CRC value into the buffer
    buffer[3 + size] = pkt->crc;

    // Send the complete buffer
    int length = send(socket_fd, buffer, total_length, 0);
    free(buffer);  // Free the allocated buffer
    if (length == -1) {
        perror("Error sending data");
        return 0;
    }

    return 1;
}

int receive_packet(int socket_fd, packet* received_pkt) {
    unsigned char buffer[MAX_PKT_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Receive the raw packet data into buffer
    if (recv(socket_fd, buffer, MAX_PKT_SIZE, 0) < 0) {
        perror("recv function error");
        received_pkt = NULL;
        return -1;
    }

    // Check if it is a packet from the protocol
    if (buffer[0] != INIT_MARKER) {
        //printf("Received packet is not from the protocol\n");
        return 0;
    }

    // Initialize packet fields
    received_pkt->init = buffer[0];
    received_pkt->size_seq_type[0] = buffer[1];
    received_pkt->size_seq_type[1] = buffer[2];

    // Unpack size, sequence number, and type
    uint8_t size, seq, type;
    unpack_fields(received_pkt, &size, &seq, &type);

    // Copy data section if size is valid
    if (size == 0) {
        received_pkt->data = NULL;
    } else if (size > 0 && size <= MAX_DATA_SIZE) {
        received_pkt->data = (unsigned char *)malloc(size);
        if (received_pkt->data == NULL) {
            perror("Packet->data malloc failed on receive_packet");
            return -1;
        }
        memcpy(received_pkt->data, &buffer[3], size);
    } else {
        perror("Packet received has invalid size on receive_packet");
        return -1;
    }

    // Handle CRC-8
    received_pkt->crc = buffer[3 + size];
    if (!check_crc8(received_pkt)) {
        perror("CRC check failed");
        return -1;
    }

    return 1; // Successfully received and processed packet
}

void free_packet(packet *pkt) {
    if (pkt->data != NULL) {
        free(pkt->data);
        pkt->data = NULL;
    }
    
    // Zeroes entire packet
    memset(pkt, 0, sizeof(*pkt));

    return;
}
