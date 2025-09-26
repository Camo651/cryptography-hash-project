#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/sha.h>

int main()
{
    // Example usage of SHA-256
    const char *message = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message, strlen(message));
    SHA256_Final(hash, &sha256);

    printf("SHA-256 hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}