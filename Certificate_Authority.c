//
//  Certificate_Authority.c
//  skm
//
//  Created by Randy Terry on 3/24/25.
//

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
#include <poll.h>
#include "connect.c"

#define MAX_NAME_LEN 256
#define MAX_NUM_OF_CONNECTIONS 10

/* Structure Definitions*/


struct Users_Info {
    char Name[MAX_BUFFER];
    char IP_addr[16];
    char key[8];
};
struct DNS_server {
    struct Users_Info Users[MAX_NUM_OF_CONNECTIONS +1];
    struct pollfd Clients[MAX_NUM_OF_CONNECTIONS+1];
} DNS_Server;


/* Global Variables */
char hostbuffer[MAX_NAME_LEN];
char *IPbuffer = NULL;
struct hostent *host_entry;
struct sockaddr_in Server;
struct sockaddr_in * clients[MAX_NUM_OF_CONNECTIONS];
int Socket;
char action;
int Num_of_Clients;
char Target[MAX_BUFFER];
unsigned char ID_key[8];
unsigned char * Temp_key;

/* Function Prototypes*/
int whoAmI(void);
int Get_Bytes(void * dest, int NUMBYTES);
unsigned char * Make_Key(unsigned char key[8]);

int main(void){
    whoAmI();
    Socket = socket(AF_INET,SOCK_STREAM,0);
    if (socket < 0){perror("Failed to create Socket"); exit(1);}
    Server.sin_family =AF_INET;
    Server.sin_port = htons(50000);
    Server.sin_addr.s_addr = INADDR_ANY;
    
    bind(Socket,(const struct sockaddr *)&Server, sizeof(Server));
    
    listen(Socket, MAX_NUM_OF_CONNECTIONS);
    
    for( int i =0; i<=MAX_NUM_OF_CONNECTIONS; i++){
        DNS_Server.Clients[i].fd = -1;
    }
    Get_Bytes(&ID_key, 8);
    while (1){
        int c = 0;
        while( c == 0){
            if(poll(DNS_Server.Clients, MAX_NUM_OF_CONNECTIONS, 5000) !=0){c = 1;}
        };
        for( int i = 0; i<MAX_NUM_OF_CONNECTIONS; i++){
            if( DNS_Server.Clients[i].revents && POLLIN){
                read(DNS_Server.Clients[i].fd, &action, sizeof(char));
                switch(action){
                        
                        
                    case register_Device:
                        read(DNS_Server.Clients[i].fd, &DNS_Server.Users[i].Name, MAX_BUFFER);
                        read(DNS_Server.Clients[i].fd, DNS_Server.Users[i].IP_addr, 16);
                        write(DNS_Server.Clients[i].fd, &Num_of_Clients, sizeof(int));
                        Num_of_Clients++;
                        unsigned char *val = Make_Key(malloc(sizeof(char)* 8));
                        strncpy(DNS_Server.Users[i].key, (const char *)val, 8);
                        write(DNS_Server.Clients[i].fd, DNS_Server.Users[i].key, sizeof(DNS_Server.Users[i].key));
                        write(DNS_Server.Clients[i].fd, hostbuffer, sizeof(hostbuffer));
                        break;
                        
                        
                    case request_device:
                        read(DNS_Server.Clients[i].fd, &Target, sizeof(Target));
                        for( int c = 0; c< Num_of_Clients; c++){
                            if(strcmp(Target, DNS_Server.Users[c].Name) == 1){ write(DNS_Server.Clients[i].fd, DNS_Server.Users[c].Name, MAX_BUFFER); write(DNS_Server.Clients[i].fd, DNS_Server.Users[c].IP_addr, 16);}
                        }
                        
                        write(DNS_Server.Clients[i].fd, DNS_Server.Users[c].key, sizeof(char) * 8);
                        
                        break;
                        
                    case Request_KEY:
                        Temp_key= Make_Key(malloc(sizeof(char)*8));
                        write(DNS_Server.Clients[i].fd, Temp_key, 8);
                        strncpy(DNS_Server.Users[i].key, (const char *)Temp_key, 8);
                }
            }
        }
        
    }
    
    return 0;
    
}


/* Function Definitions */
int whoAmI(void){
    
    int hostname;
        // To retrieve hostname
    if (gethostname(hostbuffer, sizeof(hostbuffer)) == 0){perror("No Host Name"); exit(1);};

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

int Get_Bytes(void * dest, int NUMBYTES){
    arc4random_buf(dest, NUMBYTES);
    return 0;
};

unsigned char * Make_Key(unsigned char key[8]){
    unsigned char key1[8];
    Get_Bytes(&key1, 8);
    for (int t = 0; t<8; t++){
        key[t] = key1[t] ^ ID_key[t];
    }
    return key;
}
