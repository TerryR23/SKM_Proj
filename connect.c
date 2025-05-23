#include "Device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>


#define MAX_BUFFER 256
#define BLOCK_LEN 16
#define MAX_NUM_OF_CONNECTIONS 3


enum commands{
    register_Device = 48, /* (device Name, Device IP address)*/
    Request_Certificate = 49, /* (Device Name, Device IP address)*/
    Request_KEY = 50, /* (Device Name) */
    request_device = 51, /* (Device Name, Target Device Name) */
    New_Request = 52,
    New_Message = 53,
    
};

struct Con {
    char Name[MAX_BUFFER];
    int port_out;
    int fd_write;
    int fd_read;
    int port_in;
    char IP_addr[15];
    unsigned char ID[BLOCK_LEN];
    char Message[MAX_BUFFER * 8];
    struct sockaddr_in Client_addr;
};



/*takes in hostname and port number and returning the Client fd*/

int Make_Connection(int port_num, const char * dest){
    struct sockaddr_in serv_addr;
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
        exit(-1);
    }

    if ((STATUS = connect(Client_fd, (struct  sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        printf("\n Failed to connect to target server, closing program \n");
        exit(-1);
    }

    printf("Connected Successfully!\n");
    return Client_fd;
    
    }
 

