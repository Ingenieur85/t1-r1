#include <sys/time.h>

#include "define.h"


// Returns raw byte size of file
size_t get_file_size(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return 0; // Indicate error
    }
    fseek(file, 0, SEEK_END); // Move to the end of the file
    size_t file_size = (size_t)ftell(file); // Get the position (size)
    fclose(file);
    return file_size;
}

// Function to read `n` bytes from a file
size_t read_n_bytes(FILE *file, unsigned char *buffer, size_t n) {
    size_t total_read = 0, bytes_read;
    while (total_read < n && (bytes_read = fread(buffer + total_read, 1, n - total_read, file)) > 0) {
        total_read += bytes_read;
    }
    return total_read;
}


// Overwrite the target file at the start
void overwrite_file (const char *file_path) {
    FILE *output_file = fopen(file_path, "w");
    if (!output_file) {
        perror("Error opening output file for overwriting");
        return;
    }
    fclose(output_file); // Close immediately after truncating
}

// Function to write received chunks to a file
int write_to_file(const char *file_path, unsigned char *data, size_t size) {
    FILE *file = fopen(file_path, "ab");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    size_t total_written = 0, bytes_written;
    while (total_written < size) {
        bytes_written = fwrite(data + total_written, 1, size - total_written, file);
        if (bytes_written == 0) {
            perror("Error writing to file");
            fclose(file);
            return -1;
        }
        total_written += bytes_written;
    }

    fclose(file);
    return 0;
}

// Function to send a file over the network using Stop-and-Wait protocol
#define TIMEOUT_SECONDS 2 // Define the timeout duration in seconds

// Timestamp function to get the current time in milliseconds
long long timestamp() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void send_file(int socket_fd, const char *file_path) {
    size_t file_size = get_file_size(file_path);
    if (file_size == 0) { // Check for error or empty file
        fprintf(stderr, "Error determining file size or file is empty.\n");
        return;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    printf("File size: %zu bytes\n", file_size);

    unsigned char buffer[MAX_DATA_SIZE]; // Fixed-size buffer on the stack
    size_t bytes_read;
    int seq = 0; // Initial sequence number

    while ((bytes_read = fread(buffer, 1, MAX_DATA_SIZE, file)) > 0) {
        packet pkt;
        memset(&pkt, 0, sizeof(packet));
        build_packet(&pkt, bytes_read, seq, DATA, buffer);

        int ack_received = 0;
        while (!ack_received) {
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);

            // Wait for ACK with timeout
            long long start_time = timestamp();
            while (timestamp() - start_time < TIMEOUT_SECONDS * 1000) {
                packet ack_pkt;
                memset(&ack_pkt, 0, sizeof(packet));

                if (receive_packet(socket_fd, &ack_pkt)) {
                    if (get_packet_type(&ack_pkt) == ACK && get_packet_seq(&ack_pkt) == seq) {
                        ack_received = 1;
                        free_packet(&ack_pkt);
                        break;
                    }
                    free_packet(&ack_pkt);
                }
            }

            if (!ack_received) {
                printf("Timeout occurred. Retransmitting packet with sequence number %d.\n", seq);
            }
        }

        free_packet(&pkt);
        seq = (seq + 1) % MAX_SEQ_NUMBER; // Increment sequence number
    }

    fclose(file);
}

// Function to receive a file over the network using Stop-and-Wait protocol
void receive_file(int socket_fd, const char *file_path, size_t file_size) {
    overwrite_file(file_path); // Truncate file before receiving

    size_t total_bytes_read = 0;
    int expected_seq = 0; 

    while (total_bytes_read < file_size) {
        packet pkt;
        memset(&pkt, 0, sizeof(packet));
        if (receive_packet(socket_fd, &pkt)) {
            uint8_t pkt_type = get_packet_type(&pkt);

            if (pkt_type == DATA) {
                if (get_packet_seq(&pkt) == expected_seq) {
                    write_to_file(file_path, pkt.data, get_packet_size(&pkt));
                    expected_seq = (expected_seq + 1) % MAX_SEQ_NUMBER; 
                    total_bytes_read += get_packet_size(&pkt);
                }

                // Send ACK regardless of whether the packet was expected
                packet ack_pkt;
                memset(&ack_pkt, 0, sizeof(packet));
                build_packet(&ack_pkt, 0, get_packet_seq(&pkt), ACK, NULL);
                send_packet(socket_fd, &ack_pkt);
                flush_socket(socket_fd, &ack_pkt);
                free_packet(&ack_pkt);
            } else {
                //fprintf(stderr, "Error receiving packet.\n");
                continue;
            }
        }
        free_packet(&pkt);
    }

    if (total_bytes_read != file_size) {
        fprintf(stderr, "Warning: Expected %zu bytes but received %zu bytes.\n", file_size, total_bytes_read);
    }
    printf("Transferencia Completa.\n");
}


//Debugging function, trash
void read_and_print_file(const char *file_path) {
    size_t file_size = get_file_size(file_path);
    if (file_size == 0) { // Check for error or empty file
        fprintf(stderr, "Error determining file size or file is empty.\n");
        return;
    }
    
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }
    
    printf("File size: %zu bytes\n", file_size);

    unsigned char buffer[MAX_DATA_SIZE]; // Fixed-size buffer on the stack
    size_t bytes_read, total_bytes_read = 0;

    overwrite_file("RecvdLorem.txt");

    while ((bytes_read = fread(buffer, 1, MAX_DATA_SIZE, file)) > 0) {
        // Print the bytes
        for (size_t i = 0; i < bytes_read; ++i) {
            printf("%02X ", buffer[i]);
        }
        printf("\n");

        // Write the bytes to another file
        if (write_to_file("RecvdLorem.txt", buffer, bytes_read) != 0) {
            fprintf(stderr, "Error writing data to output file.\n");
            fclose(file);
            return;
        }

        total_bytes_read += bytes_read;
    }
    
    if (total_bytes_read != file_size) {
        fprintf(stderr, "Warning: Expected %zu bytes but read %zu bytes.\n", file_size, total_bytes_read);
    }

    fclose(file);
}

long long calculate_checksum(const char* my_file) {
    if (my_file == NULL) {
        perror("Invalid file path in calculate_checksum function");
        return -1; // Indicate an error
    }

    char command[256];
    snprintf(command, sizeof(command), "cksum %s", my_file);

    // Open a pipe to execute the command and read its output
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen failed");
        return -1; // Indicate an error
    }

    unsigned long calculated_checksum;
    unsigned long file_size;

    // Parse the output of the cksum command
    if (fscanf(pipe, "%lu %lu", &calculated_checksum, &file_size) != 2) {
        fprintf(stderr, "Failed to parse cksum output.\n");
        pclose(pipe);
        return -1; // Indicate an error
    }

    // Close the pipe
    pclose(pipe);

    // Return the checksum as long long
    return (long long)calculated_checksum;
}

int verify_checksum(const char* my_file, unsigned long checksum) {
    if (my_file == NULL) {
        perror("Invalid file path on verify_chesum function");
        return 0;
    }

    char command[256];
    snprintf(command, sizeof(command), "cksum %s", my_file);

    // Open a pipe to execute the command and read its output
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen failed");
        return 0;
    }

    unsigned long calculated_checksum;
    unsigned long file_size;
    int matched = 0;

    // Parse the output of the cksum command
    if (fscanf(pipe, "%lu %lu", &calculated_checksum, &file_size) == 2) {
        matched = (calculated_checksum == checksum);
        printf("Checksum recebido: %lu, Calculado: %lu, Deu match: %s\n", checksum, calculated_checksum, matched ? "Sim" : "Não");
    } else {
        fprintf(stderr, "Failed to parse cksum output.\n");
    }

    // Close the pipe
    pclose(pipe);

    return matched;
}


int file_exists(const char* file_path) {
    // Check if file_path is valid
    if (file_path == NULL) {
        return 0; // File does not exist if the path is NULL
    }

    // Use access to check file existence
    if (access(file_path, F_OK) == 0) {
        return 1; // File exists
    } else {
        return 0; // File does not exist
    }
}
