#ifndef FILE_H
#define FILE_H



size_t get_file_size(const char *file_path);
size_t read_n_bytes(FILE *file, unsigned char *buffer, size_t n);
void overwrite_file (const char *file_path);
int write_to_file(const char *file_path, unsigned char *data, size_t size);
void read_and_print_file(const char *file_path);
long long calculate_checksum(const char* my_file);
int verify_checksum(const char* my_file, unsigned long checksum);
int file_exists(const char* file_path);




#endif