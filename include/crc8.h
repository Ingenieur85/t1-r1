#ifndef CRC8_H
#define CRC8_H

unsigned char calculate_crc8(packet* pkt);
int check_crc8(packet* pkt);

#endif
