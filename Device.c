//
//  Device.c
//  skm
//
//

#include "connect.c"



/* pound-defined values*/


#define MIN_PORT_NUM 50000
#define MAX_BUFFER 256
#define BLOCK_LEN 16


/* Structure definitions*/

struct Certificate {
    int Device_Num;
    unsigned char ID_val[8];
    char Device_Name[MAX_BUFFER];
    char CA_Name[MAX_BUFFER];
} cert;

struct flags{
    _Bool port_Num;
    _Bool Host_addr;
    _Bool req_Conn;
    _Bool Test;
} flags;

struct connections {
    struct pollfd FDs[MAX_NUM_OF_CONNECTIONS+1];
    struct Con *User_info[MAX_NUM_OF_CONNECTIONS+1];
    struct sockaddr_in * Clients[MAX_NUM_OF_CONNECTIONS+1];
} Connection_Used;

/*End structures*/









/* global variables */
int port_num = 0;
char hostbuffer[MAX_BUFFER];
char *IPbuffer = NULL;
struct hostent *host_entry;
int hostname = 0;
char Certificate_Authority[MAX_BUFFER];
char targetbuffer[MAX_BUFFER];
int NumOfConnections = 1;
int LastPort;
int current = 0;
struct hostent *host_entry;
struct sockaddr_in Server, client;
int Socket;
int action;
int Num_of_Clients;
char Target[MAX_BUFFER];
unsigned char ID_key[8];
unsigned char * Temp_key;
socklen_t client_len = sizeof(Server);

/* End Variables */








/* function Prototypes*/
int set_flag(int len, const char * options[]);
int whoAmI(void);
struct Con ** CreateConnect(struct Con ** connection);
int Get_info(int CA, char *dest);
int Create_Block(char ** message);
void Commands(void);
char *itoa(int x);
int Get_Key(unsigned char *mixKey);
unsigned char * Enc(unsigned char * message, unsigned char * key);
unsigned char * Dec(unsigned char * cipher_text, unsigned char * key);
unsigned char * Block_Cipher(unsigned char message[BLOCK_LEN], unsigned char key[BLOCK_LEN +8]);
void Print_Connection(void);
int Open_Connection(int portnum, const char * dest);
int Read_Messages(void);
unsigned char * Make_Key(unsigned char key[8]);
/* End prototypes*/









int main(int argc, const char * argv[]){
    set_flag(argc, argv);
    whoAmI();
    LastPort = MIN_PORT_NUM;
    int action;
    char message[MAX_BUFFER * 8];
    char * Text = message;
     /*
    for (int i = 0; i<MAX_BUFFER; i++){
        strncpy(&hostbuffer[i], 0, sizeof(char));
        strncpy(&Certificate_Authority[i],0, sizeof(char));
        strncpy(&targetbuffer[i], 0, sizeof(char));
        strncpy(&cert.CA_Name[i], 0, sizeof(char));
    }
      */
    puts("Buffers set to 0");
    for (int i=0; i< MAX_BUFFER * 8; i++){
        message[i] = '0';
    }
    
    if(flags.Host_addr != 1){
        puts("What Certificate Authority would you like to register to?\n please format answer: 'xxx:xxx:xxx:xxx'\n Certificate IP Address: ");
        scanf("%15s",Certificate_Authority);
        flags.Host_addr = 1;
    }
    int fd_CA = Make_Connection(MIN_PORT_NUM, Certificate_Authority);
    if (fd_CA != -1){
        struct Con Temp;
        Connection_Used.User_info[1] = &Temp;
        write(fd_CA,itoa(register_Device),sizeof(char) *2);
        write(fd_CA, hostbuffer, strlen(hostbuffer));
        write(fd_CA, IPbuffer, strlen(IPbuffer));
        read(fd_CA, &cert.Device_Num, sizeof(int));
        read(fd_CA, Connection_Used.User_info[0]->ID, BLOCK_LEN/2);
        read(fd_CA, cert.ID_val, BLOCK_LEN/2);
        read(fd_CA, cert.CA_Name, sizeof(char) * MAX_BUFFER);
        strncpy(Connection_Used.User_info[1]->Name, cert.CA_Name, sizeof(char)*MAX_BUFFER);
        NumOfConnections++;
        LastPort++;
        current++;
        puts("device registered. \n");
    }
    Socket = socket(AF_INET,SOCK_STREAM,0);
    if (socket < 0){perror("Failed to create Socket"); exit(1);}
    Server.sin_family =AF_INET;
        Server.sin_port = htons(MIN_PORT_NUM + 1);
    Server.sin_addr.s_addr = INADDR_ANY;
    
    bind(Socket,(const struct sockaddr *)&Server, sizeof(Server));
    
    listen(Socket, MAX_NUM_OF_CONNECTIONS);
    
    for( int i =1; i<=MAX_NUM_OF_CONNECTIONS; i++){
        Connection_Used.FDs[i].fd = -1;
    }
    Connection_Used.FDs[0].fd = Socket;
    Connection_Used.FDs[0].events = POLLIN;
    Connection_Used.FDs[1].fd = fd_CA;
    Connection_Used.FDs[1].events = POLLIN;
    fcntl(Socket, F_SETFL, O_NONBLOCK);
    while (1){
        int status = poll(Connection_Used.FDs, MAX_NUM_OF_CONNECTIONS, 5000);
        
        if(Connection_Used.FDs[0].revents & POLLIN){
            int client_sock = accept(Socket,(struct sockaddr *) &client, &client_len);
            if (client_sock > 0){
                for (int i =1; i<MAX_NUM_OF_CONNECTIONS;i++){
                    if(Connection_Used.FDs[i].fd== -1){
                        Connection_Used.FDs[i].fd = client_sock;
                        Connection_Used.FDs[i].events = POLLIN;
                        Num_of_Clients++;
                        break;
                    } //End if statement
                } // End For loop
            } // End if statement
        } // End If statement
        
        for( int i = 0; i<MAX_NUM_OF_CONNECTIONS; i++){
            if(Connection_Used.FDs[i].fd == -1) continue;
            if( Connection_Used.FDs[i].revents & POLLIN){
                Commands();
                scanf("%1d\n", &action);
                switch(action){
                    case 1:
                        Read_Messages();
                    case 2:
                        puts("What do you want to say?\n Enter message: ");
                        scanf("%s",message);
                        Create_Block(&Text);
                        write(Connection_Used.FDs[i].fd, Enc((unsigned char *)Text, Connection_Used.User_info[i]->ID), MAX_BUFFER);
                        break;
                    case 3:
                        Print_Connection();
                        puts("Who would you like to Talk to? \n Enter Number: ");
                        scanf("%d",&current);
                        puts("What do you want to say?\n Enter message: ");
                        scanf("%s",message);
                        Create_Block(&Text);
                        write(Connection_Used.FDs[i].fd, Enc((unsigned char *)Text, Connection_Used.User_info[i]->ID), MAX_BUFFER);
                        break;
                        
                    case 4:
                        /* Registering a new device to talk to*/
                        CreateConnect(&Connection_Used.User_info[NumOfConnections]);
                        puts("Who would you like to talk to?");
                        scanf("%255s",targetbuffer);
                        if (Get_info(fd_CA, targetbuffer) != 0){perror("get info error"); exit(1);};
                        Connection_Used.FDs[NumOfConnections].fd = Make_Connection(LastPort, Connection_Used.User_info[NumOfConnections]->IP_addr);
                        if (Connection_Used.FDs[NumOfConnections].fd == -1){perror("Make Connection failure"); exit(1);}
                        else{ LastPort++;}
                        
                        
                    case 5:
                        puts("Closing Program now... GoodBye!\n");
                        exit(0);
                }
                   
                }
            }
        }
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
    if (gethostname(hostbuffer, sizeof(hostbuffer)) == -1){perror("No Host Name"); exit(1);};

        // To retrieve host information
        host_entry = gethostbyname(hostbuffer);

        // To convert an Internet network
        // address into ASCII string
        IPbuffer = inet_ntoa(*((struct in_addr*)
                            host_entry->h_addr_list[0]));

        printf("Hostname: %s\n", hostbuffer);
        printf("Host IP: %s\n", IPbuffer);

        return 0;
}

struct Con ** CreateConnect(struct Con ** connection){
    static int Num;
    struct Con temp;
    *connection = &temp;
    /*
    for(int i= 0; i< MAX_BUFFER; i++){
        strncpy(&temp.Name[i], NULL, sizeof(char));
    }
    for(int i=0;i<BLOCK_LEN;i++){
        strncpy((char *)&temp.ID[i], NULL, sizeof(char));
    }
    for(int i = 0; i<MAX_BUFFER * 8; i++){
        strncpy(&temp.Message[i], NULL, sizeof(char));
    }*/
    
    return connection;
}


int Get_info(int CA, char * dest){
    if(&dest[0] != NULL){
        unsigned char mixkey[BLOCK_LEN/2];
        write(CA, itoa(request_device), sizeof(char));
        write(CA, hostbuffer, MAX_BUFFER); /* Not needed by the Hosting server*/
        write(CA,targetbuffer,MAX_BUFFER);
        read(CA, Connection_Used.User_info[NumOfConnections]->Name, sizeof(char)*MAX_BUFFER);
        read(CA, Connection_Used.User_info[NumOfConnections]->IP_addr, sizeof(char) * 16);
        read(CA, mixkey, sizeof(char) * 8);
        Get_Key(mixkey);

    }
    return 0;
}

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


void Commands(void){
    printf("Connection to %s created successfully. What would you like to do?\n", Connection_Used.User_info[current]->Name);
    printf("%d\n", current);
    puts("select from the options below:\n");
    puts("Option 1: Read incoming Messages. \n");
    puts("Option 2: send a message to the connected device.\n");
    puts("Option 3: Change the device you are talking to\n");
    puts("Option 4: Register a new connection\n");
    puts("Option 5: Quit the Progam\n");
    return;
}

/* look into converting an interger into a sring the safe and smart way */
char * itoa(int x){
    switch(x){
        case 48:
            return "48";
        case 49:
            return "49";
        case 50:
            return "50";
        case 51:
            return "51";
        case 52:
            return "52";
        case 53:
            return "53";
    }
    return NULL;
}


int Get_Key(unsigned char * mixKey){
    unsigned char *temp = Connection_Used.User_info[current]->ID;
    unsigned char key[BLOCK_LEN/2];
    unsigned char * ptr = key;
    for (int i = 0; i < (BLOCK_LEN/2); i++){
        key[i] = mixKey[i] ^ Connection_Used.User_info[current]->ID[i];
    }
    strncpy((char *)temp, (char *)ptr, (BLOCK_LEN/2));
    return 1;
}

unsigned char * Enc(unsigned char * message, unsigned char * key){
    unsigned char * cipher_text = (unsigned char *)malloc(strlen((const char *)message) * sizeof(char));
    unsigned char buffer[16];
    unsigned char * ptr = buffer;
    unsigned char * temp;
    int count = (int)strlen((const char *) message);
    int i =0;
    while( i< count){
        temp = &message[i];
        strncpy((char *)ptr, (const char *)temp, 16);
        strncat((char *)cipher_text, (const char *)Block_Cipher(buffer, key), 16);
        i+=16;
        
    }
    /* Add the portion to carry the cipher text as a new value to be used in the next call*/
    
    return cipher_text;
}

unsigned char * Dec(unsigned char * cipher_text, unsigned char *key){
    unsigned char * message = (unsigned char *)malloc(strlen((const char*)cipher_text) * sizeof(char));
    unsigned char buffer[16];
    unsigned char * ptr = buffer;
    unsigned char * temp;
    int count = (int)strlen((const char *)cipher_text);
    for( int i = 0; i< count; i+=16 ){
        temp = &cipher_text[i];
        strncpy((char *)ptr, (const char *)temp, 16);
        strncat((char *)message, (const char *)Block_Cipher(buffer, key), 16);
    }
    
    return message;
}

unsigned char * Block_Cipher(unsigned char message[BLOCK_LEN], unsigned char key[BLOCK_LEN +8]){
    
    unsigned char * cipher = (unsigned char *)malloc(BLOCK_LEN *sizeof(char));
    for (int i = 0; i< BLOCK_LEN; i++){
        cipher[i] = message[i] ^ key[i];
        
    }
    return cipher;
}


void Print_Connection(void){
    if(NumOfConnections <= 1 ){puts("No Unread Messages to Read"); return;}
    for (int i = 0; i< NumOfConnections; i++){
        printf("connection 1: %s\n", Connection_Used.User_info[current]->Name);
    }
    return;
}


int Read_Messages(void){
    for (int i = 0; i < NumOfConnections; i++){
        read(Connection_Used.FDs[i+1].fd, Connection_Used.User_info[current]->Message, MAX_BUFFER * 8);
        unsigned char * message = Dec((unsigned char *)&Connection_Used.User_info[current]->Message, Connection_Used.User_info[current]->ID);
        printf("Message %d (sent by %s): %s",i,Connection_Used.User_info[current]->Name, message);
    }
    return 1;
}


unsigned char * Make_Key(unsigned char key[8]){
    unsigned char key1[8];
    for (int t = 0; t<8; t++){
        key[t] = key1[t] ^ Connection_Used.User_info[current]->ID[t];
    }
    return key;
}
