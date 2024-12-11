#include "print.h"


void printMenu() {
    printf("\n=== MENU DE OPÇÕES ===\n");
    printf("1. backup <nome_arquivo>\n");
    printf("2. restaura <nome_arquivo>\n");
    printf("3. verifica <nome_arquivo>\n");
    printf("4. sair\n");
}

void print_binary(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
}

void print_size_seq_type(uint8_t size_seq_type[2]) {
    // Extract pack size (6 bits), seq (5 bits), and type (5 bits)
    uint16_t combined = (size_seq_type[0] << 8) | size_seq_type[1]; // Combine the two bytes

    // Pack size (first 6 bits)
    uint8_t pack_size = (combined >> 10) & 0x3F;  // 6 bits
    // Seq (next 5 bits)
    uint8_t seq = (combined >> 5) & 0x1F;         // 5 bits
    // Type (last 5 bits)
    uint8_t type = combined & 0x1F;               // 5 bits


    printf("Pack size (6 bits): ");
    for (int i = 5; i >= 0; i--) printf("%d", (pack_size >> i) & 1);
    printf("\n");

    printf("Seq (5 bits): ");
    for (int i = 4; i >= 0; i--) printf("%d", (seq >> i) & 1);
    printf("\n");

    printf("Type (5 bits): ");
    for (int i = 4; i >= 0; i--) printf("%d", (type >> i) & 1);
    printf("\n");
}


void print_packet(packet pkt) {
    printf("\nInit (8 bits): ");
    print_binary(pkt.init);
    printf("\n");

    //printf("Size, Seq, Type:\n");
    print_size_seq_type(pkt.size_seq_type);

    printf("CRC (8 bits): ");
    print_binary(pkt.crc);
    printf("\n");


    if (pkt.data != NULL) {
        for (int i = 0; i < get_packet_size(&pkt); i++) {
            printf("%02X", pkt.data[i]);
        }
    } else {
        printf("Data: (NULL)\n");
    }
    printf("\n");
}