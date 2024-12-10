#include "define.h"

void build_packet(packet *pkt, uint8_t size, uint8_t seq, uint8_t type, const unsigned char *data) {
    if (pkt == NULL) return;

    // Free old data if it exists
    if (pkt->data != NULL) {
        free(pkt->data);
        pkt->data = NULL;
    }

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
    pkt->crc = calculate_crc8(pkt);
}

int send_packet(int socket_fd, packet *pkt) {
    if (pkt == NULL) {
        fprintf(stderr, "Invalid packet pointer\n");
        return 0;
    }

    // Extract size, sequence, and type fields
    uint8_t size, seq, type;
    unpack_fields(pkt, &size, &seq, &type);

    // Calculate the total packet size
    int total_length = sizeof(pkt->init) + sizeof(pkt->size_seq_type) + sizeof(pkt->crc);
    if (size > 0 && pkt->data != NULL) {
        total_length += size;
    }

    // Ensure the packet meets the minimum size requirement
    int padded_length = total_length < MIN_PACKET_SIZE ? MIN_PACKET_SIZE : total_length;

    // Allocate buffer for the packet
    unsigned char *buffer = (unsigned char *)malloc(padded_length);
    if (buffer == NULL) {
        perror("Buffer malloc failed");
        return 0;
    }

    // Fill the buffer with packet data
    buffer[0] = pkt->init;
    buffer[1] = pkt->size_seq_type[0];
    buffer[2] = pkt->size_seq_type[1];

    if (size > 0 && pkt->data != NULL) {
        memcpy(&buffer[3], pkt->data, size);
    }

    // Place the CRC after the actual data
    buffer[3 + size] = pkt->crc;

    // Add padding if required
    if (padded_length > total_length) {
        memset(&buffer[total_length], 0, padded_length - total_length);
    }

    // Send the buffer
    int sent_length = send(socket_fd, buffer, padded_length, 0);
    free(buffer);

    if (sent_length == -1) {
        perror("Error sending packet");
        return 0;
    } else if (sent_length != padded_length) {
        fprintf(stderr, "Incomplete packet sent: %d/%d bytes\n", sent_length, padded_length);
        return 0;
    }

    return 1;
}

int receive_packet(int socket_fd, packet* received_pkt) {
    if (received_pkt == NULL) return -1;

    unsigned char buffer[MAX_PKT_SIZE];
    memset(buffer, 0, sizeof(buffer));

    free_packet(received_pkt); // Clean up any old data

    int bytes_received = recv(socket_fd, buffer, MAX_PKT_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv function error");
        return -1;
    }

    if (buffer[0] != INIT_MARKER) {
        // Received packet does not match protocol
        return 0;
    }

    received_pkt->init = buffer[0];
    received_pkt->size_seq_type[0] = buffer[1];
    received_pkt->size_seq_type[1] = buffer[2];

    uint8_t size, seq, type;
    unpack_fields(received_pkt, &size, &seq, &type);

    build_packet(received_pkt, size, seq, type, (size > 0) ? &buffer[3] : NULL);

    if (received_pkt->crc != buffer[3 + size]) {
        perror("CRC check failed");
        free_packet(received_pkt); // Clean up on failure
        return -1;
    }

    return 1;
}

void free_packet(packet *pkt) {
    if (pkt == NULL) return;
    if (pkt->data != NULL) {
        free(pkt->data);
        pkt->data = NULL;
    }
    memset(pkt, 0, sizeof(*pkt));

    return;
}


