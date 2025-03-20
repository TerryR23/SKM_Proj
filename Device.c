//
//  Device.c
//  skm
//
//  Created by Randy Terry on 3/19/25.
//

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
#include "connect.c"


/* pound-defined values*/

#define MAX_CONNECTIONS 3
#define MIN_PORT_NUM 50000
#define MAX_BUFFER 256
enum commands{
    Request_Certificate, /* (Device Name, Device IP address)*/
    Request_KEY, /* (Device Name) */
    request_device, /* (Device Name, Target Device Name) */
    
};

/* Structure definitions*/

struct Certificate {
    int Device_Num;
    unsigned char ID_val[8];
    char Device_Name[30];
    char CA_Name[30];
} cert;

struct flags{
    _Bool port_Num;
    _Bool Host_addr;
    _Bool req_Conn;
    _Bool Test;
} flags;


struct Con {
    char Name[MAX_BUFFER];
    int port_out;
    int port_in;
    char IP_addr[15];
    unsigned char ID[8];
};

/*End structures*/









/* global variables */
int port_num = 0;
char hostbuffer[256];
char *IPbuffer = NULL;
struct hostent *host_entry;
int hostname = 0;
char Certificate_Authority[256];
struct Con * connections[MAX_CONNECTIONS];
char targetbuffer[MAX_BUFFER];
int NumOfConnections;
int LastPort;
/* End Variables */









/* function Prototypes*/
int set_flag(int len, const char * options[]);
int whoAmI(void);
struct con * CreateConnection(void);
int Get_info(int CA, char *dest);

/* End prototypes*/









int main(int argc, const char * argv[]){
    LastPort = MIN_PORT_NUM;
    for (int i = 0; i<MAX_CONNECTIONS;i++){
        connections[i] = NULL;
    }
    for (int i =0; i<256; i++){
        strncpy(&hostbuffer[i], NULL, sizeof(char));
        strncpy(&Certificate_Authority[i], NULL, sizeof(char));
        strncpy(&targetbuffer[i], NULL, sizeof(char));
    }
    
    set_flag(argc, argv);
    whoAmI();
    if(flags.Host_addr != 1){
        puts("What Certificate Authority would you like to register to?\n please format answer: 'xxx:xxx:xxx:xxx'\n Certificate IP Address: ");
        scanf("%15s",Certificate_Authority);
        flags.Host_addr = 1;
    }
    int fd_CA = Make_Connection(MIN_PORT_NUM, Certificate_Authority);
    if (fd_CA != 0){
        NumOfConnections++;
        LastPort++;
        write(fd_CA, hostbuffer, strlen(hostbuffer));
        write(fd_CA, IPbuffer, strlen(IPbuffer));
        read(fd_CA, &cert.Device_Num, sizeof(int));
        read(fd_CA, cert.ID_val, sizeof(char) * 8);
        read(fd_CA, cert.Device_Name, sizeof(char) * 30);
        read(fd_CA, cert.CA_Name, sizeof(char) * 30);
    }else{
        perror("invalid IP Address"); exit(1);
    }
   
    puts("Who would you like to talk to?");
    scanf("%255s",targetbuffer);
    if (Get_info(fd_CA, targetbuffer) != 0){perror("get info error"); exit(1);};
    Make_Connection(connections[NumOfConnections]->port_out, connections[NumOfConnections]->IP_addr);
    
    return 0;
    
}








/* function Definitions*/
int set_flag(int len, const char * options[]){
    if( len > 1) {
        for (int i =0; i< len; i++){
            char opt = *(options[i] + 1);
            switch(opt){
                case 'P':
                    port_num = *options[i+1];
                    flags.port_Num = 1;
                    break;
                case 'H':
                    strncpy(Certificate_Authority, options[i+1], strlen(options[i+1]));
                    flags.Host_addr = 1;
                    break;
                case 'C':
                    flags.req_Conn = 1;
                    break;
                case 'T':
                    flags.Test = 1;
                    break;
            }
        }
    }
    else {return -1;}
    
    return 1;
}


int whoAmI(void){

        // To retrieve hostname
        hostname = gethostname(hostbuffer, sizeof(hostbuffer));

        // To retrieve host information
        host_entry = gethostbyname(hostbuffer);

        // To convert an Internet network
        // address into ASCII string
        IPbuffer = inet_ntoa(*((struct in_addr*)
                            host_entry->h_addr_list[0]));

        printf("Hostname: %s\n", hostbuffer);
        printf("Host IP: %s", IPbuffer);

        return 0;
}

struct Con *CreateConnect(void){
    static struct Con *connection;
    struct Con temp;
    connection = &temp;
    temp.port_out = LastPort;
    temp.port_in = LastPort +1;
    LastPort += 2;
    return connection;
}
int Get_info(int CA, char * dest){
    struct Con temp;
    if(&dest[0] != NULL){
        write(CA, "2", sizeof(char));
        write(CA, hostbuffer, MAX_BUFFER);
        write(CA,targetbuffer,MAX_BUFFER);
        read(CA, temp.Name, sizeof(char)*MAX_BUFFER);
        read(CA, temp.IP_addr, sizeof(char) * MAX_BUFFER);
        read(CA, temp.ID, sizeof(char) * 8);
        connections[NumOfConnections -1] = &temp;
    }
    return 0;
}
