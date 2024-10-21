#include "define.h"

uint8_t get_packet_size(packet *pkt) {
    uint8_t size, seq, type = 0;
    unpack_fields(pkt, &size, &seq, &type);
    return size;
}

void build_packet(packet *pkt, uint8_t size, uint8_t seq, uint8_t type, const unsigned char *data) { 
    pkt->init = INIT_MARKER;
    pack_fields(pkt, size, seq, type);

/*
    if (size > MAX_DATA_SIZE || strlen((const char *)data) > MAX_DATA_SIZE) {
        printf("Trying to send packet with more than %d Bytes!", MAX_DATA_SIZE);
        return;
    }
*/
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
    // CRC-8 calculation over size_seq_type and data fields
    unsigned char crc_input[2 + size]; // 2 bytes for size_seq_type + size bytes for data
    memcpy(crc_input, pkt->size_seq_type, 2);  // Copy size_seq_type field (2 bytes)
    memcpy(crc_input + 2, pkt->data, size);    // Copy data field (size bytes)
*/
    pkt->crc = calculate_crc8(pkt);
    return;
}
/*
int receive_packet(packet* received_pkt) {
    unsigned char buffer[MAX_PKT_SIZE] = 0;

    if ((recv(socket_fd, received_pkt, MAX_PKT_SIZE, 0)) < 0) {
        perror("recv function error");
        return NULL;
    }
    // Check if it is a packet from the protocol
    if (received_pkt->init == INIT_MARKER) {
        //do something
    }
    if (check_crc8(received_pkt, sizeof(received_pkt), received_pkt->crc)) {
        //do something
    }
    

}
*/
void free_packet(packet *pkt) {
    if (pkt->data != NULL) {
        free(pkt->data);
        pkt->data = NULL;
    }
    
    // Zeroes entire packet
    memset(pkt, 0, sizeof(*pkt));

    return;
}
