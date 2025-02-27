//
//  main.c
//  Simple-Key-Management
//
//  Created by Randy Terry on 10/8/24.
//

#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <strhash.h>
#include <unistd.h>
#include <sys/random.h>
#include <stdlib.h>
#include "AES.c"
#include "connect.c"




#define HASH_Algo "SHA256"
#define MAX_MES 300

struct Certificate {
    char Device_Num;
    unsigned char ID_Num;
    char Device_Name[30];
    char CA_Name[30];
    char *Key_Derivation_Function;
    
};
struct Device {
    int Device_Num;
    unsigned char ID_num;
    const char KDF;
    unsigned char Key[32];
    char Device_Name[30];
    char * Connections[];
};

struct Cert_Auth{
    int Device_Num;
    unsigned char ID_Num;
    int port_Num;
    char Device_Name[30];
    int Num_of_Devices;
    struct Device * Devices[5];

};


int main( int argc, const char * argv[]);
int Create_Device(struct Cert_Auth * CA, char NAME[27]);
int Get_Bytes(unsigned char *dest, int NUMBYTES);

int block_Cipher( int messagelen);

int AES_CBC(unsigned char * text, int messagelen, unsigned char * cipher);
int Get_Message(unsigned char * message);




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
                
            case 'D':
                puts("Running in Device mode");
                set_mode = 2;
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
        Device.KDF = HASH_Algo;
        Get_Bytes(Device.Key, 32);
        char message[MAX_MES];
        if ((Get_Message(message)) =!0){puts("Error getting message \n");return -1;};
        if ((AES_Block(message, Device.key)) =! 0){puts("error encryting message \n"); return -1;};

    }
    

    printf("Hello, World!\n");
    return 0;
}

/* This function is called when a device reaches out. Used by the CA, this creates and fills a Certificate and returns it to the device*/
int Create_Device(struct Cert_Auth * CA, char NAME[]){
    struct Certificate new_cert;
    new_cert.Device_Num = CA->Num_of_Devices + 2;
    if ((strcpy(new_cert.CA_Name, CA->Device_Name)) != 0){ puts("Error Copying Certificate Authority Name \n"); return -1;};
    if ((strcpy(new_cert.Device_Name, NAME)) != 0){puts("Error Copying Device Name \n"); return -1;};
    if ((Get_Bytes(&new_cert.ID_Num, 1)) != 0){puts("Error Retriving Bytes\n"); return -1;};
    new_cert.Key_Derivation_Function = HASH_Algo;
    
    return 0;
};

int Get_Bytes(unsigned char *dest, int NUMBYTES){
    arc4random_buf(dest, NUMBYTES);
    return 0;
};



int AES_CBC(unsigned char * text, int messagelen, unsigned char * cipher){
    unsigned char temp[16];
    unsigned char * current = temp;
    int CycleCount;
    unsigned char * MesSt;
    unsigned char * MesFin;
    for( CycleCount = 0; CycleCount < (messagelen/32); CycleCount++){
        memcpy((char *) current, (char *) &text[16*CycleCount], 16);
        
        
    }
    
    
    
    return 0;
};

int Get_Message(unsigned char * message){
    char temp[MAX_MES];
    scanf("%s", temp);
    strcpy((char *) message, temp);
    return 0;
};
