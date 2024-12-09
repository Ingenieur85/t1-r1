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
    int socket_fd = cria_raw_socket(INTERFACE);
    printf("Starting client on interface: %s\n", INTERFACE);


    packet pkt;
    memset(&pkt, 0, sizeof(packet));
    const unsigned char test = 'A';
    uint8_t seq = 30;
    seq = ((seq + 1) & 0b00011111);

    build_packet(&pkt, strlen((const char *)error_message[3]), seq, ERROR, error_message[2]);


    //build_packet(&pkt, 0, seq, NACK, NULL);
    
    print_packet(pkt);
    send_packet(socket_fd, &pkt);


    //const char *file_path = "/home/fds/redes1/t1/lorem.txt";
    //read_and_print_file(file_path);


    //printf("RESULT: %d", verify_checksum(file_path, 637994836));

    //free_packet(&pkt);

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

        // BACKUP
        if (strncmp(command, "backup", 6) == 0) {
            sscanf(command, "backup %s", file_name);


        // RESTAURA
        } else if (strncmp(command, "restaura", 8) == 0) {
            sscanf(command, "restaura %s", file_name);


        // VERIFICA
        } else if (strncmp(command, "verifica", 8) == 0) {
            sscanf(command, "verifica %s", file_name);
            printf("\n");

            snprintf(file_path, sizeof(file_path), "%s/%s", client_files, file_name);

            printf("\n OII %s", file_path);
            if (!file_exists(file_path)) {
                printf("Arquivo não encontrado.Tente Novamente\n");
                continue;
            }

            build_packet(&pkt, strlen((const char *)file_name), 0, CHECK, (unsigned char *)file_name);
            print_packet(pkt);
            send_packet(socket_fd, &pkt);

            if (receive_packet(socket_fd, &pkt)) {
                if (get_packet_type(&pkt) == OKCHECKSUM) {
                    int checksum = 0;
                    memcpy(&checksum, pkt.data, sizeof(int));
                    verify_checksum(file_name, checksum);
                } else if (get_packet_type(&pkt) == ERROR) {
                    printf("OIEEEEEE%s", pkt.data);
                } else {
                    perror("Pacote de resposta não esperado.\n");
                }
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
