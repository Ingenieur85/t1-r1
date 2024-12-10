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
#include "rawsoc.h"

#define INTERFACE "lo"
#define MAX_DATA_SIZE 63
#define MAX_PKT_SIZE 90 // 8 + 6 + 5 + 63 + 8 = 90
#define MIN_PACKET_SIZE 14
#define BUFFER_SIZE 1024
#define SERVER_DIR "server_files"
#define CLIENT_DIR "client_files"
#define MAX_FILESIZE 1024 * 1000 * 1000

#define INIT_MARKER 0b01111110
#define MAX_SEQ_NUMBER 31
#define MAX_RETRIES 128
#define TIMEOUT_MILISEC 3999

// Client side packet types
#define ACK        0b00000
#define NACK       0b00001
#define BACKUP     0b00100  
#define RESTORE    0b00101
#define CHECK      0b00110 // Asks for Server's checksum on a file
#define FILESIZE   0b01111 // Sends filesize to server

// Server side packet types
#define DATA       0b10000
#define ENDTX      0b10001 // End of data transmission
#define OK         0b00010 // Works as a begin transmission marker
#define OKCHECKSUM 0b01101 // Server Responds with checksum for file on data field
#define OKSIZ      0b01110 // Server responds with filesize for a file
#define ERROR      0b11111

//#define SEQ { sequencia = (sequencia + 1) % (31); }
//extern int sequencia, socket_fd;
//extern int socket_fd;

typedef struct packet_t {
    uint8_t init;             // 8-bits
    uint8_t size_seq_type[2]; //  size (6 bits), seq (5 bits), type (5 bits)
    unsigned char* data;      // 0-63 bytes
    uint8_t crc;              // 8-bits
} packet;

// ERROR codes (CANNOT be > 63 Bytes !)
#define ERROR_1 "SERVIDOR: Erro Desconhecido"
#define ERROR_2 "SERVIDOR: Sem acesso ao arquivo"
#define ERROR_3 "SERVIDOR: Sem espaço para armazenar arquivo"
#define ERROR_4 "SERVIDOR: Arquivo não encontrado no servidor"

// Own libs
#include "crc8.h"
#include "bitpack.h"
#include "print.h"
#include "file.h"

// Functions from packet.c 
void build_packet(packet *pkt, uint8_t size, uint8_t seq, uint8_t type, const unsigned char *data);
int send_packet(int socket_fd, packet *pkt);
int receive_packet(int socket_fd, packet* received_pkt);
//void write_data_to_file(FILE *fp, packet pkt);
void free_packet(packet *pkt);
void flush_socket(int socket_fd, const packet *last_sent_pkt);



#endif