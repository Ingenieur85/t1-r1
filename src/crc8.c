#include "define.h"

#define CRC_POLYNOMIAL 0x07 // CRC-8 polynomial (x^8 + x^2 + x^1 + 1)

// Function to calculate 8-bit CRC
unsigned char calculate_crc8(unsigned char *data, size_t length) {
    unsigned char crc = 0; // Initial CRC value
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i]; // XOR data byte into CRC

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

// Function to check if received data is valid (using CRC)
int check_crc8(unsigned char *data, size_t length, unsigned char received_crc) {
    unsigned char calculated_crc = calculate_crc8(data, length);
    return calculated_crc == received_crc; // Return 1 if valid, 0 otherwise
}
