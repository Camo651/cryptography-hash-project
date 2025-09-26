#include <stdio.h>
#include <string.h>
#include "clean_sha.h"

void hex(const unsigned char *d, size_t n)
{
     for(size_t i=0;i<n;i++) 
     printf("%02x", d[i]); 
}

int main() 
{
    const char *msg = "Hello World";
    unsigned char out[32];
    sha256_ctx c; 
    
    sha256_init(&c);
    sha256_update(&c, msg, strlen(msg));
    sha256_final(&c, out);
    
    hex(out, 32); puts("");
    
    return 0;
}
