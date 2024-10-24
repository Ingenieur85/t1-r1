#include "print.h"


// Function to print binary representation of a byte (8 bits)
void print_binary(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
}

// Function to extract and print the bits from size_seq_type
void print_size_seq_type(uint8_t size_seq_type[2]) {
    // Extract pack size (6 bits), seq (5 bits), and type (5 bits)
    uint16_t combined = (size_seq_type[0] << 8) | size_seq_type[1]; // Combine the two bytes

    // Pack size (first 6 bits)
    uint8_t pack_size = (combined >> 10) & 0x3F;  // 6 bits
    // Seq (next 5 bits)
    uint8_t seq = (combined >> 5) & 0x1F;         // 5 bits
    // Type (last 5 bits)
    uint8_t type = combined & 0x1F;               // 5 bits

    // Print the individual components in binary
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

// Function to print the packet fields in binary
void print_packet(packet pkt) {
    printf("\nInit (8 bits): ");
    print_binary(pkt.init);
    printf("\n");

    //printf("Size, Seq, Type:\n");
    print_size_seq_type(pkt.size_seq_type);

    printf("CRC (8 bits): ");
    print_binary(pkt.crc);
    printf("\n\n");

    // Print the data (assuming it's a null-terminated string for simplicity)
    if (pkt.data != NULL) {
        printf("Data: %s\n", pkt.data);
    } else {
        printf("Data: (NULL)\n");
    }
}