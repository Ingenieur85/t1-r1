#include "define.h"

#define CRC_POLYNOMIAL 0x07 // CRC-8 polynomial (x^8 + x^2 + x^1 + 1)


unsigned char calculate_crc8(packet* pkt) {

    unsigned char crc = 0; // Initial CRC value
    uint8_t s = get_packet_size(pkt);

    unsigned char crc_input[2 + s]; // 2 bytes for size_seq_type + size bytes for data
    memcpy(crc_input, pkt->size_seq_type, 2);  // Copy size_seq_type field (2 bytes)
    memcpy(crc_input + 2, pkt->data, s);    // Copy data field (size bytes)

    for (size_t i = 0; i < sizeof(crc_input); i++) {
        crc ^= crc_input[i]; // XOR data byte into CRC

        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL; // Apply polynomial
            } else {
                crc <<= 1; // Shift left
            }
        }
    }
    return crc;
}

int check_crc8(packet* pkt) {
    unsigned char calculated_crc = calculate_crc8(pkt);
    return calculated_crc == pkt->crc; // Return 1 if valid, 0 otherwise
}
