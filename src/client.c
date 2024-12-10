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

/*
    packet pkt;
    memset(&pkt, 0, sizeof(packet));
    const unsigned char test = 'A';
    uint8_t seq = 30;
    seq = ((seq + 1) & 0b00011111);

    build_packet(&pkt, strlen((const char *)error_message[3]), seq, ERROR, error_message[2]);


    //build_packet(&pkt, 0, seq, NACK, NULL);
    
    print_packet(pkt);
    send_packet(socket_fd, &pkt);
    free_packet(&pkt);


    //const char *file_path = "/home/fds/redes1/t1/lorem.txt";
    //read_and_print_file(file_path);


    //printf("RESULT: %d", verify_checksum(file_path, 637994836));

    //free_packet(&pkt);
*/

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
                printf("Comando inválido. Use: backup <nome_do_arquivo>\n");
                continue;
            }
            printf("Cheguei aqui");
            packet pkt;
            memset(&pkt, 0, sizeof(packet));

            snprintf(file_path, sizeof(file_path), "%s/%s", client_files, file_name);

            if (!file_exists(file_path)) {
                printf("Arquivo não encontrado. Tente novamente.\n");
                continue;
            }



            //Manda pedido de backup
            build_packet(&pkt, strlen(file_name), 0, BACKUP, (unsigned char *)file_name);
            //print_packet(pkt);
            send_packet(socket_fd, &pkt);
            flush_socket(socket_fd, &pkt);
            free_packet(&pkt);
            memset(&pkt, 0, sizeof(packet));

            // Escuta resposta do servidor
            while (1) {
                if (receive_packet(socket_fd, &pkt)) {
                    uint8_t pkt_type = get_packet_type(&pkt);

                    // Manda FILESIZE
                    if (pkt_type == OK) {
                        size_t f_size = get_file_size(file_path);
                        build_packet(&pkt, strlen(file_name), 0, FILESIZE,(const unsigned char *) f_size);
                        send_packet(socket_fd, &pkt);
                        flush_socket(socket_fd, &pkt);
                        free_packet(&pkt);
                        memset(&pkt, 0, sizeof(packet));
                        // Envia arquivo
                        while (1) {
                            if (receive_packet(socket_fd, &pkt)) {
                                uint8_t pkt_type = get_packet_type(&pkt);
                                if (pkt_type == OK) {
                                    printf("Envia o arquivo\n");

                                // Erro de envio de arquivo    
                                } else if (pkt_type == ERROR) {
                                    if (pkt.data) {
                                        printf("Erro: %.*s\n", pkt.size_seq_type[0] >> 2, pkt.data);
                                    } else {
                                        printf("Erro desconhecido.\n");
                                    }
                                    break;

                                } else if (pkt_type == ENDTX) {
                                    printf("Arquivo enviado com sucesso.\n");
                                    break;
                                } else {
                                    continue;
                                    perror("Pacote de resposta não esperado.\n");
                                }
                            }
                        }
                        break;
                    } else if (pkt_type == ERROR) {
                        if (pkt.data) {
                            printf("Erro: %.*s\n", pkt.size_seq_type[0] >> 2, pkt.data);
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
            free_packet(&pkt);
        }


        // RESTAURA
        else if (strncmp(command, "restaura", 8) == 0) {
            sscanf(command, "restaura %s", file_name);

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
