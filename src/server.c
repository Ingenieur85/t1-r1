#include "define.h"


int main() {

/*
int argc, char *argv[]
    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        printf("Example: %s eth0\n", argv[0]);
        return 1;
    }
*/
    // Deals with file directories
    char server_files[1024];
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(server_files, sizeof(server_files), "%s/%s", cwd, SERVER_DIR);
        printf("\nServer files directory: %s\n", server_files);
    } else {
        perror("getcwd");
        return 1;
    }

    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(INTERFACE);
    printf("Starting server on interface: %s\n", INTERFACE);

    printf("%s", error_message[3]);

    char file_name[MAX_DATA_SIZE];
    char file_path[2048];

    packet pkt;
    memset(&pkt, 0, sizeof(packet));

    while (1) {
        


        if (receive_packet(socket_fd, &pkt)) {

            if (get_packet_type(&pkt) == ERROR){
                print_packet(pkt);
            }

            // CHECK : Client asked for a checksum. data seciton contains filename
            if (get_packet_type(&pkt) == CHECK) {
                print_packet(pkt);
                int checksum = 0;
                memcpy(file_name, pkt.data, get_packet_size(&pkt));
                snprintf(file_path, sizeof(file_path), "%s/%s", server_files, file_name);
                checksum = calculate_checksum(file_path);

                if (!file_exists(file_path)) {
                    build_packet(&pkt, sizeof(int), 0, ERROR, (unsigned char *)"HAIL SATAN");
                } else{
                    build_packet(&pkt, sizeof(int), 0, OKCHECKSUM, (unsigned char *)&checksum);
                }
                print_packet(pkt);
                send_packet(socket_fd, &pkt);
            }



            // BACKUP


            // RESTAURA
              
        }

        
    }

    free_packet(&pkt);




    close(socket_fd);
    return 0;
}
