#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


#define SERVER_DIR "server_files"

// Function to create a string with all filenames in the directory
char* create_filenames_string(const char* directory) {
    DIR *dir;
    struct dirent *entry;
    char *filenames = malloc(1024); // Start with a 1KB buffer
    if (!filenames) {
        perror("malloc");
        return NULL;
    }

    filenames[0] = '\0'; // Initialize the string as empty

    dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        free(filenames);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." directories
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Reallocate buffer if needed
            if (strlen(filenames) + strlen(entry->d_name) + 2 > 1024) {
                char *new_filenames = realloc(filenames, strlen(filenames) + strlen(entry->d_name) + 1024);
                if (!new_filenames) {
                    perror("realloc");
                    free(filenames);
                    closedir(dir);
                    return NULL;
                }
                filenames = new_filenames;
            }
            // Append filename and a delimiter (e.g., newline)
            strcat(filenames, entry->d_name);
            strcat(filenames, "\n");
        }
    }

    closedir(dir);
    return filenames;
}

// Function to parse the filenames string and print one per line
void parse_and_print_filenames(const char* filenames) {
    if (!filenames) {
        fprintf(stderr, "No filenames string provided.\n");
        return;
    }

    char *filenames_copy = strdup(filenames); // Duplicate the string for tokenization
    if (!filenames_copy) {
        perror("strdup");
        return;
    }

    char *token = strtok(filenames_copy, "\n");
    while (token) {
        printf("%s\n", token);
        token = strtok(NULL, "\n");
    }

    free(filenames_copy);
}

int main() {
    // Define the full path to the server_files directory
    char cwd[1024];
    char server_files_path[2048];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }
    snprintf(server_files_path, sizeof(server_files_path), "%s/%s", cwd, SERVER_DIR);

    // Generate filenames string
    char *filenames = create_filenames_string(server_files_path);
    if (filenames) {
        printf("Generated Filenames String:\n%s\n", filenames);

        // Parse and print filenames
        printf("Parsed Filenames:\n");
        parse_and_print_filenames(filenames);

        free(filenames); // Clean up
    }

    return 0;
}
