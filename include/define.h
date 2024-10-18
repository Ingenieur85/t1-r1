#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>

#include "crc8.h"
#include "rawsoc.h"

#define INTERFACE "lo"
#define MAX_DATA_SIZE 63
#define BUFFER_SIZE 1028

#define INIT_MARKER 0b01111110
#define MAX_RETRIES 128
#define TIMEOUT_MILISEC 3999



// Client side packet types
#define ACK        0b00000
#define NACK       0b00001
#define BACKUP     0b00100  
#define RESTORE    0b00101
#define CHECK      0b00110 // Asks for Server's checksum on a file
#define FILESIZE   0b01111 // Asks for filesize

// Server side packet types
#define DATA       0b10000
#define ENDTX      0b10001 // End of data transmission
#define OK         0b00010 // Works as a begin transmission marker
#define OKCHECKSUM 0b01101 // Server Responds with checksum for file on data field
#define OKSIZ      0b01110 // Server responds with filesize for a file
#define ERROR      0b11111

// ERROR codes (CANNOT be > 63 Bytes !)
static const unsigned char* error_message[] = {
    (const unsigned char *) "Erro Desconhecido",                        
    (const unsigned char *) "Sem acesso ao arquivo",                
    (const unsigned char *) "Sem espaço para armazenar arquivo",        
    (const unsigned char *) "Arquivo não encontrado no servidor"
};

//#define SEQ { sequencia = (sequencia + 1) % (31); }
//extern int sequencia, socket_fd;

typedef struct packet_t {
    uint8_t init;
    uint8_t size_seq_type[2]; // Pack size (6 bits), seq (5 bits), type (5 bits)
    unsigned char* data;
    uint8_t crc;
} packet;

/*
enum t_comando {
    ls,
    rls,
    cd,
    rcd,
    get,
    put,
};

typedef enum t_comando comando;
*/

// Functions from packet.c
void build_packet(packet *pkt, uint8_t size, uint8_t seq, uint8_t type, const unsigned char *data);
void free_packet(packet *pkt);

// bitpack.c
void pack_fields(packet *pkt, uint8_t size, uint8_t seq, uint8_t type);
void unpack_fields(packet *pkt, uint8_t *size, uint8_t *seq, uint8_t *type);

// print.c
void print_binary(uint8_t byte);
void print_size_seq_type(uint8_t size_seq_type[2]);
void print_packet(packet pkt);

#endif