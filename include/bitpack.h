#ifndef BITPACK_H
#define BITPACK_H

uint8_t get_packet_size(packet *pkt);
uint8_t get_packet_seq(packet *pkt);
uint8_t get_packet_type(packet *pkt);
void pack_fields(packet *pkt, uint8_t size, uint8_t seq, uint8_t type);
void unpack_fields(packet *pkt, uint8_t *size, uint8_t *seq, uint8_t *type);

#endif