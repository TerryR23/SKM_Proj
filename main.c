//
//  main.c
//  Simple-Key-Management
//
//  Created by Randy Terry on 10/8/24.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define HASH_Algo "SHA256"
#define MAX_MES 300
#define MAX_CONNECTIONS 10

struct Certificate {
    char Device_Num;
    unsigned char ID_Num;
    char Device_Name[30];
    char CA_Name[30];
    char Key_Derivation_Function = HASH_Algo;
    
};
struct Device {
    int Device_Num;
    unsigned char ID_num;
    unsigned char Key[32];
    char Device_Name[30];
    char * Connections[2];
    struct Certificate * Cert;
};

struct Cert_Auth{
    int Device_Num;
    unsigned char ID_Num;
    int port_Num;
    char Device_Name[30];
    int Num_of_Devices;
    void * DNS = struct DNS_Server;
};



struct DNS_Server{
    struct connection * connections[MAX_CONNECTIONS];
};

struct connection{
    char name[30];
    int port;
    unsigned char IP[4];
};


int Create_Device(struct Cert_Auth * CA, char NAME[27]);
int Get_Bytes(unsigned char *dest, int NUMBYTES);
int Get_Message(unsigned char * message);
unsigned char * Request_Connection(char name[30], struct Cert_Auth * CA);




int main(int argc, const char * argv[]) {
    int set_mode = 0;
        if (argc <2){
            puts("Error, missing possitional arguement. Please try again");
            return -1;
        }
       /* Reads command line to see if it's the device or CA (will be removed when different devices used) */
    
    for (int i =0; i< argc; i++){
        char flag = *argv[i];
        printf("%c",flag);
        switch (flag){
            case 'C':
                puts("Running as a Certificate Authority");
                set_mode = 1;
                break;
                
            case 'D':
                puts("Running in Device mode");
                set_mode = 2;
                break;
        }
        
    }
    if (set_mode == 0){
        puts("Mode not identified, please try again");
        return -1;
    }
    
    if (set_mode == 1){
        struct Cert_Auth Host;
        Get_Bytes((unsigned char *) Host.Device_Name, 30);
        Get_Bytes(&Host.ID_Num, 1);
        Host.port_Num = 65000;
        Host.Device_Num = 1;
        Host.Num_of_Devices = 0;
    }
    
    if (set_mode == 2){
        struct Device Device;
        Get_Bytes((unsigned char *) Device.Device_Name, 30);
        Get_Bytes(&Device.ID_num, 1);
        Device.Device_Num = 1;
        if ((Get_Message(message)) != 0){puts("Error getting message \n");return -1;};
        char message[MAX_MES];
        if ((Get_Message(message)) != 0){puts("Error getting message \n");return -1;};
        if ((AES_Block(message, Device.Cert->Key_Derivation_Function)) != 0){puts("error encryting message \n"); return -1;};
        Make_Connection(50000, /* create the code needed to retrieve an IP address*/)

    }
    

    printf("Hello, World!\n");
    return 0;
}

/* This function is called when a device reaches out. Used by the CA, this creates and fills a Certificate and returns it to the device*/
int Create_Device(struct Cert_Auth * CA, char NAME[]){
    struct Certificate new_cert;
    new_cert.Device_Num = CA->Num_of_Devices + 2;
    if (strcpy(new_cert.CA_Name, CA->Device_Name) == NULL){ puts("Error Copying Certificate Authority Name \n"); return -1;};
    if (strcpy(new_cert.Device_Name, NAME) == NULL){puts("Error Copying Device Name \n"); return -1;};
    if ((Get_Bytes(&new_cert.ID_Num, 1)) != 0){puts("Error Retriving Bytes\n"); return -1;};
    
    return 0;
};

int Get_Bytes(unsigned char *dest, int NUMBYTES){
    arc4random_buf(dest, NUMBYTES);
    return 0;
};

int Get_Message(unsigned char * message){
    char temp[MAX_MES];
    scanf("%299s", temp);
    strncpy((char *) message, temp, MAX_MES);
    return 0;
};

unsigned char * Request_Connection(char name[30], struct Cert_Auth * CA){
    int i;

    for (i = 0; i < CA->Num_of_Devices; i++){
        if (strcmp(name, CA->DNS->connections[i]->name) == 0 ){
            return CA->DNS->connections[i]->IP;
        }
    } 
    printf("device not found...Please try again\n");
    return NULL;
};
