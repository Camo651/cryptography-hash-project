# include <stdio.h>
# include <stdint.h>
# include <stdbool.h>
# include <errno.h>
# include <stdlib.h>
# include <string.h>

struct Option
{
    char* key;
    bool in_string  = false;
    bool dhk        = false;    // double hash with key mode
    bool memory     = false; // memory boost mode   
};

void hex(const unsigned char * d)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", d[i]);
    }
}

void usage_message(const char *msg)
{
    if (msg && *msg)
        fprintf(stderr, "Error: %s\n\n", msg);

    fprintf(stderr,
        "Usage: ./sha256 [Flags] <file_name 1> ... <file_name n>\n\n"
        "Flags:\n"
        "  -k <secret_string>       Enable DHK mode, input the passphrase following\n"
        "  -m                       Turn on the memory boost mode\n"                
        "  -h, --help               Show usage\n",
    );

    exit(1);
}

Option parse(int argc, char* argv[])
{
    Option opt;

    if (argc == 1)
    {
        in_string = true;
    }

    for (int i = 2; i < argc; i++)

}