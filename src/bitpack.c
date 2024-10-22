#include "define.h"

uint8_t get_packet_size(packet *pkt) {
    uint8_t size, seq, type = 0;
    unpack_fields(pkt, &size, &seq, &type);
    return size;
}

uint8_t get_packet_seq(packet *pkt) {
    uint8_t size, seq, type = 0;
    unpack_fields(pkt, &size, &seq, &type);
    return seq;
}

uint8_t get_packet_type(packet *pkt) {
    uint8_t size, seq, type = 0;
    unpack_fields(pkt, &size, &seq, &type);
    return type;
}

void pack_fields(packet *pkt, uint8_t size, uint8_t seq, uint8_t type) {
    pkt->size_seq_type[0] = 0b00000000;
    pkt->size_seq_type[1] = 0b00000000;
   
    // Left shift A by 2 bits, right shift B 4 bits and take 2 MSBs only.
    pkt->size_seq_type[0] = (size << 2) | ((seq >> 4) & 0b00000011);

    //Left shift seq by 5 bits, leaving the 3 LSBs, combine with all bits of type
    pkt->size_seq_type[1] = (seq << 5) | (type & 0b00011111 );
}

void unpack_fields(packet *pkt, uint8_t *size, uint8_t *seq, uint8_t *type) {
    // Extract 6 bits of the first byte
    *size = pkt->size_seq_type[0] >> 2;

    // Extract the 2 MSBs of seq from the lower 2 bits of the first byte
    uint8_t seq_msb = (pkt->size_seq_type[0] & 0b00000011);

    // Extract the 3 LSBs of seq from the upper 3 bits of the second byte
    uint8_t seq_lsb = (pkt->size_seq_type[1] >> 5) & 0b00000111;

    // Combine seq_msb and seq_lsb to reconstruct the original 5-bit seq
    *seq = (seq_msb << 3) | seq_lsb;

    // Extract type from the lower 5 bits of the second byte
    *type = pkt->size_seq_type[1] & 0b00011111;
}
