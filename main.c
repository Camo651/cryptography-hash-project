#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "clean_sha.h"

void hex(const unsigned char * d)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", d[i]);
    }
}

static int parse_hex(const char *hex, uint8_t **out, size_t *outlen){
    size_t L=strlen(hex); if(L==0||(L&1)) return 0;
    uint8_t *b=malloc(L/2); if(!b) return 0;
    for(size_t i=0;i<L/2;i++){ unsigned v; if(sscanf(hex+2*i,"%2x",&v)!=1){ free(b); return 0; } b[i]=(uint8_t)v; }
    *out=b; *outlen=L/2; return 1;
}

static int read_all_stdin(uint8_t **buf, size_t *len){
    size_t cap=1<<16, n=0; uint8_t *p=malloc(cap); if(!p) return 0;
    for(;;){
        if(n==cap){ cap<<=1; uint8_t *q=realloc(p,cap); if(!q){ free(p); return 0; } p=q; }
        size_t g=fread(p+n,1,cap-n,stdin); n+=g; if(g==0) break;
    }
    *buf=p; *len=n; return 1;
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
        if(!fp)
        { 
            fprintf(stderr, "error: %s: %s\n", path, strerror(errno)); 
            return 1; 
        }
    }
    
    int ok = SHA256_Stream(fp, out);
    
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
        //printf("SHA256(%s)= ", path); 
        hex(out);
        printf("  %s",path); 
        putchar('\n');
    }

    return 0;
}


int main(int argc, char** argv)
{

/*    // File input
    if (argc==1) 
    {
        return hash_path("-");            // read stdin
    }
    
    int rc = 0;
    
    for(int i = 1; i < argc; i++)
    { 
        rc |= hash_path(argv[i]);
    }

    return rc; */

    if(argc!=2){ fprintf(stderr,"usage: sha_key <hexkey>  < infile\n"); return 1; }

    uint8_t *msg=NULL; size_t mlen=0;
    if(!read_all_stdin(&msg,&mlen)){ fprintf(stderr,"read error\n"); return 2; }

    uint8_t *key=NULL; size_t klen=0;
    if(!parse_hex(argv[1], &key, &klen)){ fprintf(stderr,"bad hex key\n"); free(msg); return 3; }

    uint8_t out[32];
    SHA256_DHK(key, klen, msg, mlen, out);
    hex(out);

    // scrub
    for(size_t i=0;i<mlen;i++) msg[i]=0; free(msg);
    for(size_t i=0;i<klen;i++) key[i]=0; free(key);
    return 0;

}