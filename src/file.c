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
    if (!file || !buffer) {
        return 0;
    }
    return fread(buffer, 1, n, file);
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
    FILE *file = fopen(file_path, "ab"); // Open file in append mode
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Write the entire data buffer to the file in one call
    if (fwrite(data, 1, size, file) != size) {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}


// Function to read an entire file, print its bytes, and write to another file
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
    } else {
        fprintf(stderr, "Failed to parse cksum output.\n");
    }

    // Close the pipe
    pclose(pipe);

    return matched;
}
