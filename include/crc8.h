#ifndef CRC8_H
#define CRC8_H

unsigned char calculate_crc8(unsigned char *data, size_t length);
int check_crc8(unsigned char *data, size_t length, unsigned char received_crc);

#endif
