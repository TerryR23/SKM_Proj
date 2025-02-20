#include <stdio.h>
#include "AES.c"

int main() {
    const char* message = "This is a test message.";
    const char* key = "0123456789abcdef0123456789abcdef"; // 32 bytes key for AES-256

    AES_Block(message, key);

    return 0;
}
