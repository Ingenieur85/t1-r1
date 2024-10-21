#ifndef BITPACK_H
#define BITPACK_H

void pack_fields(packet *pkt, uint8_t size, uint8_t seq, uint8_t type);
void unpack_fields(packet *pkt, uint8_t *size, uint8_t *seq, uint8_t *type);

#endif