#include "define.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        printf("Example: %s eth0\n", argv[0]);
        return 1;
    }

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
    int socket_fd = cria_raw_socket(argv[1]);
    printf("Starting server on interface: %s\n", argv[1]);


    char file_name[MAX_DATA_SIZE];
    char file_path[2048];



    while (1) {
        packet pkt;
        memset(&pkt, 0, sizeof(packet));

        if (receive_packet(socket_fd, &pkt)) {
            uint8_t pkt_type = get_packet_type(&pkt);



            // CHECKSUM
            if (pkt_type == CHECK) {
                //print_packet(pkt);

                size_t data_size = get_packet_size(&pkt);
                if (data_size >= sizeof(file_name)) {
                    fprintf(stderr, "Filename too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                memcpy(file_name, pkt.data, data_size);
                file_name[data_size] = '\0';

                if (strlen(server_files) + strlen(file_name) + 2 > sizeof(file_path)) {
                    fprintf(stderr, "File path too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                snprintf(file_path, sizeof(file_path), "%s/%s", server_files, file_name);

                if (!file_exists(file_path)) {
                    build_packet(&pkt, strlen(ERROR_4), 0, ERROR, (unsigned char *)ERROR_4);
                } else {
                    long long checksum = calculate_checksum(file_path);
                    //printf("Checksum: %llu\n", checksum);
                    build_packet(&pkt, sizeof(long long), 0, OKCHECKSUM, (unsigned char *)&checksum);
                }

                //printf("Sending response packet:\n");
                //print_packet(pkt);
                send_packet(socket_fd, &pkt);
                flush_socket(socket_fd, &pkt);
                free_packet(&pkt);
                printf("Esperando pelo Cliente \n");
           
            // BACKUP
            } else if (pkt_type == BACKUP) {

                size_t data_size = get_packet_size(&pkt);
                if (data_size >= sizeof(file_name)) {
                    fprintf(stderr, "Filename too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                memcpy(file_name, pkt.data, data_size);
                file_name[data_size] = '\0';

                if (strlen(server_files) + strlen(file_name) + 2 > sizeof(file_path)) {
                    fprintf(stderr, "File path too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                snprintf(file_path, sizeof(file_path), "%s/%s", server_files, file_name);
                printf("Cliente requisitou backup de: %s\n", file_name);

                // Responde OK para o cliente
                memset(&pkt, 0, sizeof(packet));
                build_packet(&pkt, 0, 0, OK, NULL);
                send_packet(socket_fd, &pkt);
                flush_socket(socket_fd, &pkt);
                free_packet(&pkt);

                // Recebe o tamanho do arquivo
                size_t file_size;

                while (1) {
                    packet received_pkt;
                    memset(&received_pkt, 0, sizeof(packet));

                    if (receive_packet(socket_fd, &received_pkt)) {
                        uint8_t pkt_type = get_packet_type(&received_pkt);

                        if (pkt_type == FILESIZE) {
                            if (received_pkt.data) {
                                memcpy(&file_size, received_pkt.data, sizeof(size_t));
                                printf("Tamanho do arquivo: %zu bytes\n", file_size);
                            } else {
                                fprintf(stderr, "Received packet unexpectedly empty.\n");
                            }
                            break;
                        } else {
                            fprintf(stderr, "Unexpected packet type: %d\n", pkt_type);
                            free_packet(&received_pkt);
                            continue;
                        }

                    }
                }
                // Manda ok para transmitir
                memset(&pkt, 0, sizeof(packet));
                build_packet(&pkt, 0, 0, OK, NULL);
                send_packet(socket_fd, &pkt);
                flush_socket(socket_fd, &pkt);
                free_packet(&pkt);


                // Recebe o arquivo
                receive_file(socket_fd, file_path, file_size);
                printf("Esperando pelo Cliente \n");


            // RESTAURA
            } else if (pkt_type == RESTORE) {

                size_t data_size = get_packet_size(&pkt);
                if (data_size >= sizeof(file_name)) {
                    fprintf(stderr, "Filename too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                memcpy(file_name, pkt.data, data_size);
                file_name[data_size] = '\0';

                if (strlen(server_files) + strlen(file_name) + 2 > sizeof(file_path)) {
                    fprintf(stderr, "File path too long.\n");
                    free_packet(&pkt);
                    continue;
                }

                snprintf(file_path, sizeof(file_path), "%s/%s", server_files, file_name);
                printf("Cliente requisitou restauro de: %s\n", file_name);
                memset(&pkt, 0, sizeof(packet));

                // Se existe, manda Ok com o tamanho
                if (!file_exists(file_path)) {
                    build_packet(&pkt, strlen(ERROR_4), 0, ERROR, (unsigned char *)ERROR_4);
                } else {
                    size_t file_size = get_file_size(file_path);
                    build_packet(&pkt, sizeof(size_t), 0, OKSIZ, (unsigned char *)&file_size);
                }

                // Responde para o cliente
                send_packet(socket_fd, &pkt);
                flush_socket(socket_fd, &pkt);
                free_packet(&pkt);

                // Espera o ok para transmitir
                packet received_pkt;
                memset(&received_pkt, 0, sizeof(packet));
                listen_for_ok(socket_fd, received_pkt, "Cliente deu OK para transmitir.");
                free_packet(&received_pkt);

                send_file(socket_fd, file_path);
                printf("Esperando pelo Cliente \n");


            // SERVIDOR RECEBEU ALGO ESTRANHO
            } else {
                continue;
                fprintf(stderr, "Unexpected packet type: %d\n", pkt_type);
                free_packet(&pkt);
            }


        }
        free_packet(&pkt);
    }


    close(socket_fd);
    return 0;
}