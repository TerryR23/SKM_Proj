//AES Block Cipher
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>        //AES library where it block size is 16 byts, 128 bits

//Sources: ChaptGPT on how to use openssl/aes.h
//         https://github.com/openssl/blob/master/include/openssl/aes.h

//AES_BLOCK_SIZE is 16      https://github.com/openssl/blob/master/include/openssl/aes.h
//AES_ENCRYPT 1
//AES_DECRYPT 0
//AES_set_encrypt_key(const unsigned char* userKey, const int bits, AES_Key *key)
//AES_cbc_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key)

//AES_set_decrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key)

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 256

int AES_Block(const char* message, const char* key) {
    size_t message_len = strlen(message); // get message length
    unsigned char EncTxt[AES_BLOCK_SIZE * ((message_len / AES_BLOCK_SIZE) + 1)] = {0}; // encrypted text
    unsigned char DecTxt[message_len + 1]; // decrypted text
    unsigned char iv[AES_BLOCK_SIZE] = {0}; // for the block chain of 16 bytes

    AES_KEY encryptKey, decryptKey;

    // Encrypt
    AES_set_encrypt_key((const unsigned char*)key, AES_KEY_SIZE, &encryptKey);
    AES_cbc_encrypt((const unsigned char*)message, EncTxt, message_len, &encryptKey, iv, AES_ENCRYPT);

    printf("Encrypted data:");
    for (size_t i = 0; i < sizeof(EncTxt); i++) {
        printf("%02x", EncTxt[i]);
    }
    printf("\n");

    // Decrypt
    AES_set_decrypt_key((const unsigned char*)key, AES_KEY_SIZE, &decryptKey);
    AES_cbc_encrypt(EncTxt, DecTxt, message_len, &decryptKey, iv, AES_DECRYPT);

    DecTxt[message_len] = '\0'; // Null-terminate the decrypted text
    printf("Decrypted data: %s\n", DecTxt);

    return 0;
}

//take size of messgage, mod by #, break down until nothing left and add padding, encrypt with key
//100%10 --> 1st 10 + key = 1st 2 slots into new arry + padding
//second 10 + key + padding

