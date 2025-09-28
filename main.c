#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "clean_sha.h"

void hex(const unsigned char *d)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", d[i]);
    }
}

int hash_path(const char *path)
{
    unsigned char out[32];
    FILE *fp;

    if (strcmp(path, "-") == 0)
    {
        fp = stdin;
    }
    else
    {
        fp = fopen(path, "rb");
        if (!fp)
        {
            fprintf(stderr, "error: %s: %s\n", path, strerror(errno));
            return 1;
        }
    }

    int ok = SHA256_Stream(fp, out);

    if (fp != stdin)
        fclose(fp);

    if (!ok)
    {
        fprintf(stderr, "error: read %s\n", path);
        return 1;
    }

    // OpenSSL-like line
    if (strcmp(path, "-") == 0)
    {
        hex(out);
        printf("  -");
        putchar('\n'); // openssl prints just hex for stdin
    }
    else
    {
        // printf("SHA256(%s)= ", path);
        hex(out);
        printf("  %s", path);
        putchar('\n');
    }

    return 0;
}

int main(int argc, char **argv)
{
    // File input
    if (argc == 1)
    {
        return hash_path("-"); // read stdin
    }

    int opt;
    size_t salt = 0;
    while ((opt = getopt(argc, argv, "sh?")) != -1)
    {
        switch (opt)
        {
        case 's':
            // salt = 1;
            // generate a random salt and save it to the salt variable
            int randomNumber = rand() % 10000; // Random number between 0 and 9999
            salt = (size_t)randomNumber;
            printf("Generated salt: %zu\n", salt);
        case 'h':
        case '?':
            fprintf(stderr, "Usage: %s [-sh] [file...]\n", argv[0]);
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "Usage: %s [-sh] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int rc = 0;

    for (int i = optind; i < argc; i++)
    {
        rc |= hash_path(argv[i]);
    }

    return rc;
}