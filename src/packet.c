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
        memset(pkt->data, 0, size);
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
    
    // Base packet size: init, size_seq_type (2 bytes), and crc (1 byte)
    int total_length = sizeof(pkt->init) + sizeof(pkt->size_seq_type) + sizeof(pkt->crc);

    // If the packet has data, add the data size to the total length
    if (size > 0 && pkt->data != NULL) {
        total_length += size;
    }

    printf("Total length: %d\n", total_length);  // Debug: print the correct total length
    printf("Data size: %d\n", size);

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

    // Copy the CRC value into the buffer (after the data if data exists)
    buffer[3 + size] = pkt->crc;

    if (total_length < MIN_PACKET_SIZE) {
        int padding = MIN_PACKET_SIZE - total_length;
        memset(&buffer[total_length], 0, padding);
        total_length += padding;
    }

/*
    // Print buffer for debugging (print each byte in hex)
    for (int i = 0; i < total_length; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
*/

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

    free_packet(received_pkt);

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

    // Print buffer for debugging (print each byte in hex)
    for (int i = 0; i < (int)sizeof(buffer); i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Initialize packet fields
    received_pkt->init = buffer[0];
    received_pkt->size_seq_type[0] = buffer[1];
    received_pkt->size_seq_type[1] = buffer[2];

    // Unpack size, sequence number, and type
    uint8_t size, seq, type;
    unpack_fields(received_pkt, &size, &seq, &type);

    build_packet(received_pkt, size, seq, type, &buffer[3]);

    if (received_pkt->crc != buffer[3 + size]) {
        perror("CRC check failed");
        return -1;
    }

#if 0
    // Copy data section if size is valid
    if (size == 0) {
        received_pkt->data = NULL;
    } else if (size > 0 && size <= MAX_DATA_SIZE) {
        received_pkt->data = (unsigned char *)malloc(size);
        if (received_pkt->data == NULL) {
            perror("Packet->data malloc failed on receive_packet");
            return -1;
        }
        printf("Size: %d\n", size);
        memcpy(received_pkt->data, &buffer[3], size);
/*
        for (int i = 0; i < size; i++) {
            received_pkt->data[i] = buffer[3 + i];
        }
*/
        fprintf(stderr, "DATA BEFORE CRC: %s\n", received_pkt->data);
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

    printf("Size DEPOIS DO CRC: %d\n", size); 
    printf("Received data:\n");
    for (int i = 0; i < size; i++) {
        printf("%02X ", received_pkt->data[i]);
    }
    printf("\n");

    fprintf(stderr, "PACKET DATA STDERR: %s\n", received_pkt->data);
#endif
    return 1; // Successfully received and processed packet
}

void write_data_to_file(FILE *fp, packet pkt) {
    // Check if the packet or file pointer is NULL
    if (fp == NULL) {
        fprintf(stderr, "Invalid file pointer\n");
        return;
    }

    uint8_t size = get_packet_size(&pkt);

    // Check if the data section is non-null and the size is valid
    if (pkt.data != NULL && size > 0) {
        // Write the data section to the file
        size_t written = fwrite(pkt.data, sizeof(unsigned char), size, fp);
        
        // Check if the correct number of bytes were written
        if (written != size) {
            fprintf(stderr, "Error writing data to file, written %zu bytes, expected %d bytes\n", written, size);
        } else {
            printf("Successfully written %d bytes to file.\n", size);
        }
    } else {
        fprintf(stderr, "No data to write or invalid size.\n");
    }
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
