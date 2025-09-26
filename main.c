#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "clean_sha.h"

void hex(const unsigned char *d, size_t n)
{
     for(size_t i=0;i<n;i++) 
     printf("%02x", d[i]); 
}

int main()
{
    // Example usage of SHA-256
    const char *message = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_ctx sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message, strlen(message));
    SHA256_Final(&sha256, hash);

    //printf("SHA-256 hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}