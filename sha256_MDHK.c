# include <stdio.h>
# include <stdint.h>
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <stdbool.h>

# include "modified_sha.h"
# include "prompt_key.h"

void hex(const unsigned char * d)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", d[i]);
    }
}

int hash_path1(const char *path, bool key, const char* pass)
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
        if(!fp)
        { 
            fprintf(stderr, "error: %s: %s\n", path, strerror(errno)); 
            return 1; 
        }
    }

    int ok;

    if (key)
    {
        ok = SHA256_DHK_Stream(fp, pass, sizeof(pass), out);
    }
    else
    {
        ok = SHA256_Stream(fp, out);
    }

    if (fp!=stdin) fclose(fp);
    
    if(!ok)
    { 
        fprintf(stderr, "error: read %s\n", path); 
        return 1; 
    }

    // OpenSSL-like line
    if (strcmp(path, "-")==0) 
    {
        hex(out);
        printf("  -"); 
        putchar('\n');    // openssl prints just hex for stdin
    } 
    else 
    {
        hex(out);
        printf("  %s",path); 
        putchar('\n');
    }

    return 0;
}

int hash_path(const char *path, bool use_key, const void *key, size_t key_len)
{
    unsigned char out[32];
    FILE *fp = (strcmp(path,"-")==0) ? stdin : fopen(path,"rb");
    if (!fp) { fprintf(stderr,"error: %s: %s\n", path, strerror(errno)); return 1; }

    int ok = use_key ? SHA256_DHK_Stream(fp, key, key_len, out)
                     : SHA256_Stream(fp, out);

    if (fp != stdin) fclose(fp);
    if (!ok) { fprintf(stderr,"error: read %s\n", path); return 1; }

    hex(out);
    printf("  %s\n", strcmp(path,"-")==0 ? "-" : path);
    return 0;
}

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [-h] [-k] [files...]\n"
        "  -h    Show this help\n"
        "  -k    Prompt for passphrase before hashing\n"
        "If no files are given, read stdin.\n", prog);
}

int main(int argc, char** argv)
{
    int opt; bool use_key = false; char *pass = NULL;

    while ((opt = getopt(argc, argv, "hk")) != -1) {
        if (opt == 'h') { usage(argv[0]); return 0; }
        else if (opt == 'k') use_key = true;
        else { usage(argv[0]); return 2; }
    }

    size_t plen = 0;
    if (use_key) {
        if (prompt_passphrase(1, "sha256", &pass) != 0) {
            fprintf(stderr,"failed to read passphrase\n"); return 1;
        }
        plen = strlen(pass);
    }

    int rc = 0;
    if (optind == argc) {
        rc |= hash_path("-", use_key, pass, plen);
    } else {
        for (int i = optind; i < argc; i++)
            rc |= hash_path(argv[i], use_key, pass, plen);
    }

    if (pass) { memset(pass, 0, plen); free(pass); }
    return rc; 

}