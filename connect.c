#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/*takes in hostname and port number and returning the Client fd*/

int Make_Connection(int port_num, unsigned char * dest){
    struct sockaddr_in serv_addr;
    char host[] = "192.0.0.1";
    int Client_fd, STATUS; 

    if (port_num <= 0 || port_num > 65535){
        printf("Invalid port #");
        return -1;
    }

    if ((Client_fd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        printf("\n Socket failed to create, closing program \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr.s_addr = inet_addr(dest);

    if (inet_pton(AF_INET, dest, &serv_addr.sin_addr) <= 0){
        printf("\n Host not found, closing program \n");
        return -1;
    }

    if ((STATUS = connect(Client_fd, (struct  sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        printf("\n Failed to connect to target server, closing program \n");
        return -1;
    }

    printf("Connected Successfuly!\n");
    return Client_fd;
    
    }
    