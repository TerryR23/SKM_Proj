#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int Make_Connection(int port_num, char * host, unisgned char * dest);
int Make_Connection(int argc, const char * argv[]){
    struct sockaddr_in serv_addr;

    int Client_fd, STATUS;
    
    if (argc != 3) {
        printf("Usage: %s <IP Address> <Port>", argv[0]);
        return -1;
    }

    char *HOST = argv[1];
    int PORT = atoi(argv[2]);

    if (PORT <= || PORT > 65535){
        printf("Invalid port #");
        return -1;
    }

    if ((Client_fd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        printf("\n Socket failed to create, closing program \n");
        return -1;
    }

    serv_addr.sinfamily = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0){
        printf("\n Host not found, closing program \n");
        return -1;
    }

    if ((STATUS = connect(Client_fd, (struct  sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        printf("\n Failed to connect to target server, closing program \n");
        return -1;
    }

    printf("Connected Successfuly!\n");
    return 0;
    
    };