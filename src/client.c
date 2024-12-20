#include "define.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        printf("Example: %s eth0\n", argv[0]);
        return 1;
    }
 
    char client_files[1024];
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(client_files, sizeof(client_files), "%s/%s", cwd, CLIENT_DIR);
        printf("\nClient files directory: %s\n", client_files);
    } else {
        perror("getcwd");
        return 1;
    }

    // Create the raw socket using the loopback interface (lo)
    int socket_fd = cria_raw_socket(argv[1]);
    printf("Starting client on interface: %s\n", argv[1]);


    char command[256];
    char file_name[MAX_DATA_SIZE];
    char file_path[2048];
    //packet pkt;
    int running = 1;

    while (running) {
        printMenu();
        printf("\nDigite um comando: ");
        fgets(command, sizeof(command), stdin);

        // Remove newline character if present
        command[strcspn(command, "\n")] = '\0';


        // VERIFICA
        if (strncmp(command, "verifica", 8) == 0) {
            if (sscanf(command, "verifica %255s", file_name) != 1) {
                printf("Comando inválido. Use: verifica <nome_do_arquivo>\n");
                continue;
            }

            packet pkt;
            memset(&pkt, 0, sizeof(packet));

            snprintf(file_path, sizeof(file_path), "%s/%s", client_files, file_name);

            if (!file_exists(file_path)) {
                printf("Arquivo não encontrado. Tente novamente.\n");
                continue;
            }

            build_packet(&pkt, strlen(file_name), 0, CHECK, (unsigned char *)file_name);
            //print_packet(pkt);
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);

            free_packet(&pkt); // Avoid memory leaks

            packet received_pkt;
            memset(&received_pkt, 0, sizeof(packet));

            while (1) {

                if (receive_packet(socket_fd, &received_pkt)) {
                    uint8_t pkt_type = get_packet_type(&received_pkt);
                    //printf("Received packet type: %d\n", pkt_type);
                    //print_packet(received_pkt);

                    if (pkt_type == OKCHECKSUM) {
                        long long checksum = 0;
                        memcpy(&checksum, received_pkt.data, sizeof(long long));
                        verify_checksum(file_path, checksum);
                        break;
                    } else if (pkt_type == ERROR) {
                        if (received_pkt.data) {
                            printf("Erro: %.*s\n", received_pkt.size_seq_type[0] >> 2, received_pkt.data);
                        } else {
                            printf("Erro desconhecido.\n");
                        }
                        break;
                    } else {
                        continue;
                        perror("Pacote de resposta não esperado.\n");
                    }
                }
            }
            free_packet(&received_pkt); // Clean up after receiving
        

        // BACKUP
        } else if (strncmp(command, "backup", 6) == 0) {
            if (sscanf(command, "backup %255s", file_name) != 1) {
                printf("Comando inválido. Use: verifica <nome_do_arquivo>\n");
                continue;
            }

            packet pkt;
            memset(&pkt, 0, sizeof(packet));

            snprintf(file_path, sizeof(file_path), "%s/%s", client_files, file_name);

            if (!file_exists(file_path)) {
                printf("Arquivo não encontrado. Tente novamente.\n");
                continue;
            }

            // Pede o backup do arquivo
            build_packet(&pkt, strlen(file_name), 0, BACKUP, (unsigned char *)file_name);
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);
            free_packet(&pkt);

            // Escuta a resposta
            packet received_pkt;
            memset(&received_pkt, 0, sizeof(packet));
            listen_for_ok(socket_fd, received_pkt, "Servidor tem o nome do arquivo.");
            free_packet(&received_pkt); // Clean up after receiving

            // Envia tamanho do arquivo
            size_t file_size = get_file_size(file_path);
            printf("Tamanho do arquivo: %zu bytes\n", file_size);
            memset(&pkt, 0, sizeof(packet));
            build_packet(&pkt, sizeof(size_t), 0, FILESIZE, (unsigned char *)&file_size);
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);
            free_packet(&pkt);

            // Recebe Ok para transmitir arquivo
            memset(&received_pkt, 0, sizeof(packet));
            listen_for_ok(socket_fd, received_pkt, "Servidor recebeu o tamanho do arquivo e deu ok.");
            free_packet(&received_pkt);

            // Transmite o arquivo
            send_file(socket_fd, file_path);


        // RESTAURA
        } else if (strncmp(command, "restaura", 8) == 0) {
            if (sscanf(command, "restaura %255s", file_name) != 1) {
                printf("Comando inválido. Use: verifica <nome_do_arquivo>\n");
                continue;
            }

            int error_flag = 0;
            packet pkt;
            memset(&pkt, 0, sizeof(packet));

            snprintf(file_path, sizeof(file_path), "%s/%s", client_files, file_name);

            // Pede o restauro do arquivo
            build_packet(&pkt, strlen(file_name), 0, RESTORE, (unsigned char *)file_name);
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);
            free_packet(&pkt);

            // Escuta a resposta para ver se arquivo existe no servidor
            size_t file_size;
            packet received_pkt;
            memset(&received_pkt, 0, sizeof(packet));
            while (1) {

                if (receive_packet(socket_fd, &received_pkt)) {
                    uint8_t pkt_type = get_packet_type(&received_pkt);

                    if (pkt_type == OKSIZ) {
                        if (received_pkt.data) {
                            memcpy(&file_size, received_pkt.data, sizeof(size_t));
                            printf("Tamanho do arquivo a ser restaurado: %zu bytes\n", file_size);
                        } else {
                            fprintf(stderr, "Received packet unexpectedly empty.\n");
                        }
                        break;
                    } else if (pkt_type == ERROR) {
                        if (received_pkt.data) {
                            printf("Erro: %.*s\n", received_pkt.size_seq_type[0] >> 2, received_pkt.data);
                            error_flag = 1;
                            break;
                        } else {
                            printf("Erro desconhecido.\n");
                            break;
                        }

                    } else {
                        continue;
                        perror("Pacote de resposta não esperado.\n");
                    }
                }
            }
            free_packet(&received_pkt);

            if (!error_flag) {
                // Envia Ok para transmitir arquivo

                memset(&pkt, 0, sizeof(packet));
                build_packet(&pkt, 0, 0, OK, NULL);
                send_packet(socket_fd, &pkt);
                flush_socket(socket_fd, &pkt);
                free_packet(&pkt);

                // Recebe o arquivo
                receive_file(socket_fd, file_path, file_size);
            }

        // SAIR
        } else if (strcmp(command, "sair") == 0) {
            printf("Encerrando o programa!\n");
            running = 0;
        } else {
            printf("Comando inválido. Tente novamente.\n");
        }
    }

    close(socket_fd);
    return 0;

}