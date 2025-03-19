//
//  main.c
//  Simple-Key-Management
//
//  Created by Randy Terry on 10/8/24.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "connect.c"




#define HASH_Algo "SHA256"
#define MAX_MES 300
#define MAX_CONNECTIONS 10
#define MIN_PORT 50000
#define BLOCK_LEN 16

struct Certificate {
    int Device_Num;
    unsigned char ID_val[8];
    char Device_Name[30];
    char CA_Name[30];
    char Key_Derivation_Function[15];
    
};
struct Cert_Auth {
    int Device_Num;
    unsigned char ID_Num[8];
    int port_Num;
    char Device_Name[30];
    int Num_of_Devices;
    struct DNS_Server * DNS;
};
struct Device {
    int Num_of_connections;
    char Device_Name[30];
    struct connection * Connections[2];
    struct Certificate * Cert;
    struct Cert_Auth * CA;
};

struct DNS_Server{
    struct connection * connections[MAX_CONNECTIONS]; /* connections will be iterated using port numbers connections{portnum:struct connection}*/
};

struct connection{
    char name[30];
    unsigned char IP[4];
    int port_Num;
    unsigned char key[24];
};

int Create_CertAuth(struct Cert_Auth * CA, struct DNS_Server * DNS);

int Create_Device(char Name[],struct Device * Target);

int Get_Bytes(void * dest, int NUMBYTES);

int Get_Message(char message[]);

unsigned char * Request_Connection(char name[30], struct Cert_Auth * CA);

int Register_Device(struct Device * Target,struct connection * Dev_connect, struct connection * Host_connect,  struct Cert_Auth * CA, struct Certificate * Cert);

int CD2H(struct Device * Dev, struct Cert_Auth * Host, struct connection * connection);

int CH2D(struct Cert_Auth * Host, struct Device * Dev, struct connection * connection);

int Load_Key(struct Certificate * Cert);

int Create_key(struct Device * Dev1, struct Device * Dev2, struct Cert_Auth * Host);

int Create_Block( char ** message);

unsigned char * Enc(unsigned char * message, unsigned char * key[BLOCK_LEN +8]);

unsigned char * Block_Cipher(unsigned char message[BLOCK_LEN], unsigned char key[BLOCK_LEN +8]);


int main(int argc, const char * argv[]){
    char message[MAX_MES];
    char flag;
    int set_mode = 0;
        if (argc <2){
            puts("Error, missing possitional arguement. Please try again");
            return -1;
        }
       /* Reads command line to see if it's the device or CA (will be removed when different devices used) */
    
    for (int i =0; i< argc; i++){
        flag = *argv[i];
    }
        switch (flag){
            case 'C':
                puts("Running as a Certificate Authority");
                set_mode = 1;
                break;
                
            case 'D':
                puts("Running in Device mode");
                set_mode = 2;
                break;

            case 'T':
                puts("Running in Test mode");
                set_mode = 3;
                break;
        
    }
    switch(set_mode){
        case 0:
            puts("Mode not identified, please try again");
            return -1;
            

        case 1:
            puts("registering as Certificate Authority... Please Standby...\n");
            struct Cert_Auth Host;
            struct DNS_Server DNS;
            Create_CertAuth(&Host, &DNS);
            printf("Certificate Authority Name: %s \n", Host.Device_Name);
            break;

        case 2:
            puts("Registering as Device on the Network.... Please Standby...\n");
            struct Device Dev;
            Create_Device("Device1", &Dev);
            printf("Device Name: %s \n", Dev.Device_Name);
            return 0;
            char Dest[30];
            Get_Bytes(&Dev.Cert->ID_val, 1);
            Dev.Cert->Device_Num = 1;
            printf("Who do you want to talk to? \n");
            scanf("%29s",Dest);
            
            if ((Get_Message(message)) != 0){puts("Error getting message \n");return -1;};
            if ((Get_Message(message)) != 0){puts("Error getting message \n");return -1;};

            break;

        case 3:
            puts("Beginning Test Now....\n");
            struct Device Test_Dev,Test_Dev2;
            struct Cert_Auth Test_Host;
            struct DNS_Server Test_DNS;
            struct connection Dev1toHost_connect, HosttoDev1_connect, Dev1toDev2_connect;
            struct connection Dev2toHost_connect, HosttoDev2_connect, Dev2toDev1_connect;
            struct Certificate Test_Cert, Test_Cert2;
            Create_CertAuth(&Test_Host, &Test_DNS);
            printf("Certificate Authority Name: %s \n", Test_Host.Device_Name);
            Create_Device("Device1", &Test_Dev);
            Create_Device("Device2", &Test_Dev2);
            printf("Device Name: %s \n", Test_Dev.Device_Name);
            puts("Attempting to register device on the network...\n");
            
            if( Register_Device(&Test_Dev, &Dev1toHost_connect, &HosttoDev1_connect, &Test_Host, &Test_Cert) != 0){puts("Error registering Device to network...\n"); return -1; };
            Register_Device(&Test_Dev2, &Dev2toHost_connect, &HosttoDev2_connect, &Test_Host, &Test_Cert2);
            
            
            printf("Device Certificate name is: %s \n", Test_Dev.Cert->Device_Name);
            printf("Device connected to: %s  on port: %d \n", Test_Dev.Connections[0]->name, Test_Dev.Connections[0]->port_Num);
            
            printf("Certificate Authority connected to: %s   on port: %d \n", Test_Host.DNS->connections[0]->name, Test_Host.DNS->connections[0]->port_Num);
            puts("Testing key generations");
            
            Test_Dev.Connections[1] = &Dev1toDev2_connect;
            Test_Dev2.Connections[1] = &Dev2toDev1_connect;
            Load_Key(Test_Dev.Cert);
            Load_Key(Test_Dev2.Cert);
            strncpy(Test_Dev.Connections[1]->name, Test_Dev2.Device_Name, 30);
            strncpy(Test_Dev2.Connections[1]->name, Test_Dev.Connections[1]->name, 30);
            if((Create_key(&Test_Dev, &Test_Dev2, &Host)) != 0){puts("Error making key."); return -1;};
            Create_key(&Test_Dev2, &Test_Dev, &Host);
            
            int convo = Make_Connection(60000, "127.0.0.1");
            printf("using fd: %d  and talking on port: %d  to IP: %s\n",convo, 60000, "127.0.0.1");
            break;
            

    };
    
  
    puts("Testing concluded...,\n");
    return 0;
};
int Create_CertAuth(struct Cert_Auth * Host, struct DNS_Server * DNS){
    strncpy(Host->Device_Name, "Certificate Authority", 21);
    Get_Bytes(&Host->ID_Num, 1);
    Host->port_Num = 65000;
    Host->Device_Num = 1;
    Host->Num_of_Devices = 0;
    Host->DNS = DNS;
    return 0;
};

int Create_Device(char Name[], struct Device * Dev){
    if((strcpy(Dev->Device_Name, Name)) == NULL){puts("Error setting device Name. Please rerun the program\n");return -1;};
    Dev->Num_of_connections = 0;
    return 0;
}
/* This function is called when a device reaches out. Used by the CA, this creates and fills a Certificate and returns it to the device*/
/*int Create_Device(struct Cert_Auth * CA, char NAME[]){
    struct Certificate new_cert;
    new_cert.Device_Num = CA->Num_of_Devices + 2;
    if (strcpy(new_cert.CA_Name, CA->Device_Name) == NULL){ puts("Error Copying Certificate Authority Name \n"); return -1;};
    if (strcpy(new_cert.Device_Name, NAME) == NULL){puts("Error Copying Device Name \n"); return -1;};
    if ((Get_Bytes(&new_cert.ID_Num, 1)) != 0){puts("Error Retriving Bytes\n"); return -1;};
    if((strcpy(new_cert.Key_Derivation_Function, HASH_Algo)) ==NULL) {puts("Error Copying Hash Algorithm\n"); return -1;};
    
    return 0;
};*/

int Get_Bytes(void * dest, int NUMBYTES){
    arc4random_buf(dest, NUMBYTES);
    return 0;
};
/* This function may be deleted ultimately. can be done using wait and semaphores*/
int Get_Message(char message[]){
    char temp[MAX_MES];
    scanf("%299s", temp);
    strncpy((char *) message, temp, MAX_MES);
    return 0;
};

unsigned char * Request_Connection(char name[30], struct Cert_Auth * Host){
    int i;

    for (i = 0; i < Host->Num_of_Devices; i++){
        if (strcmp(name, Host->DNS->connections[i]->name) == 0 ){
            return Host->DNS->connections[i]->IP;
        }
    } 
    printf("device not found...Please try again\n");
    return NULL;
};
int Register_Device( struct Device * Dev,struct connection * Dev_connect, struct connection * Host_connect,  struct Cert_Auth * Host, struct Certificate * Cert){
    Dev->Cert = Cert;
    puts("Setting Device Certificate...\n");
    Dev->CA = Host;
    puts("Setting Devices' Hosting Certificate Authority...\n");
    strncpy(Cert->Device_Name, Dev->Device_Name, strlen(Dev->Device_Name));
    Get_Bytes(Cert, 1);
    Host->Num_of_Devices +=1;
    
    if (CD2H(Dev,Host, Dev_connect) != 0){puts("error mapping connection from device to host\n"); return -1;}
    if (CH2D(Host, Dev, Host_connect) != 0){puts("Error mapping connection from Host to device \n"); return -1;};
    puts("device successfully registered\n");
    return 0;
};
int CD2H(struct Device * Dev, struct Cert_Auth * Host, struct connection * connection){
    strncpy(connection->name, Host->Device_Name, strlen(Host->Device_Name));
    connection->port_Num = MIN_PORT;
    Dev->Connections[Dev->Num_of_connections] = connection;
    return 0;
};

int CH2D(struct Cert_Auth * Host, struct Device * Dev, struct connection * connection){
    strncpy(connection->name, Dev->Device_Name, strlen(Dev->Device_Name));
    connection->port_Num = MIN_PORT;
    Host->DNS->connections[(Host->Num_of_Devices)-1] = connection;
    return 0;
};

int Load_Key(struct Certificate * Cert){
    Get_Bytes(&Cert->ID_val, 8);
    return 0;
}
/* This function belongs to the certifcate authority*/
int Create_key(struct Device * Dev1, struct Device * Dev2, struct Cert_Auth * Host){
    unsigned char key_val[24];
    struct Device * first= NULL;
    struct Device * second= NULL;
    int i, STATUS;
    for (i = 0; i< 24; i++){
        key_val[i]=0;
    }
    STATUS = 0;
    unsigned char * key_ptr =key_val;
    for(i=0 ; i<30 ; i++ ){
        if(Dev1->Device_Name[i] != Dev2->Device_Name[i]){
            if (Dev1->Device_Name[i] < Dev2->Device_Name[i]) {
                first = Dev2;
                second = Dev1;
            }else{
                first = Dev1;
                second = Dev2;
            }
        }
    }
    if( first ==NULL || second ==NULL){first = Dev1; second = Dev2;};
    strncat((char*)key_ptr, (const char*)first->Cert->ID_val, 8);
    strncat((char*)key_ptr, (const char*)second->Cert->ID_val, 8);
    strncat((char*)key_ptr, (const char*)first->CA->ID_Num, 8);
    strncpy((char*)first->Connections[1]->key, (const char *)key_ptr, 24);
    printf("Key made. The key on %s is:", Dev2->Device_Name);
    for (i=0; i<24; i++){
        printf("%hhx", first->Connections[1]->key[i]);
    }
    puts("\n");
    STATUS = 1;
    
    if( STATUS == 0){return -1;};
    return 0;
};

int Create_Block(char ** message){
    size_t len = strlen(*message);
    if ((len % BLOCK_LEN) != 0){
        len += 16 - (len % BLOCK_LEN);
    }
    char * block_mess = malloc(len * sizeof(char));
    for (int i=0;i< len;i++){
        if (i<strlen(*message)){
            strncpy(&block_mess[i], message[i], 1);
        }else{
            strncpy(&block_mess[i],"0", 1);
        }
    }strncpy(&block_mess[len], NULL, 1);
    
    *message = block_mess;
    return 0;
    };

unsigned char * Enc(unsigned char * message, unsigned char * key[BLOCK_LEN +8]){
    unsigned char * cipher_text = (unsigned char *)malloc(strlen((const char *)message) * sizeof(char));
    unsigned char buffer[16];
    unsigned char * ptr = buffer;
    unsigned char * temp;
    int count = (int)strlen((const char *) message);
    int i =0;
    while( i< count){
        temp = &message[i];
        strncpy((char *)ptr, (const char *)temp, 16);
        strncat((char *)cipher_text, (const char *)Block_Cipher(buffer, *key), 16);
        i+=16;
        
    }
    /* Add the portion to carry the cipher text as a new value to be used in the next call*/
    
    return cipher_text;
}


unsigned char * Block_Cipher(unsigned char message[BLOCK_LEN], unsigned char key[BLOCK_LEN +8]){
    
    unsigned char * cipher = (unsigned char *)malloc(BLOCK_LEN *sizeof(char));
    for (int i = 0; i< BLOCK_LEN; i++){
        cipher[i] = message[i] ^ key[i];
        
    }
    return cipher;
}
