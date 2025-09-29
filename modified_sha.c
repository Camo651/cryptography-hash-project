#include "modified_sha.h"

/* Logical functions from FIPS 180-4 part 4 (SHA-256) */
#define ROR32(x,n) ((uint32_t)((x >> (n)) | (x << (32-(n)))))
#define Ch(x,y,z)  ((x & y) ^ (~x & z))             /* part 4 */
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))    /* part 4 */
#define S0(x) (ROR32(x,2) ^ ROR32(x,13) ^ ROR32(x,22))  /* Σ0 in part 4 */
#define S1(x) (ROR32(x,6) ^ ROR32(x,11) ^ ROR32(x,25))  /* Σ1 in part 4 */
#define s0(x) (ROR32(x,7) ^ ROR32(x,18) ^ ((x) >> 3))   /* small_sigma_0 in part 4 */
#define s1(x) (ROR32(x,17) ^ ROR32(x,19) ^ ((x) >> 10)) /* small_sigma_1 in part 4 */

/* Initial hash values H(0) for SHA-256: FIPS 180-4 part 5.3.3 (table) */
const uint32_t H0[8] = 
{
  0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
  0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
};

/* Round constants K[0..63]: FIPS 180-4 part 4 (table for SHA-256) */
const uint32_t K[64] =
{
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Big-endian load/store for 32-bit words. Byte order per part 5 message parsing. */
void store_32(uint8_t *p, uint32_t v)
{ 
    p[0] = v >> 24; 
    p[1] = v >> 16; 
    p[2] = v >> 8; 
    p[3] = v; 
}

uint32_t load_32(const uint8_t *p)
{
    uint32_t load = (uint32_t)p[0] << 24;
    load = load | (uint32_t)p[1] << 16;
    load = load | (uint32_t)p[2] << 8;
    load = load | p[3];
    
    return load; 
}

void compress_imp(SHA256_ctx *c, const uint8_t block[64]) 
{
    uint32_t W[16];

    /* Message schedule W[0..63]. part 6.2 and σ0/σ1 from part 4. */
    for (int i = 0; i < 16; i++) 
    {    
        W[i] = load_32(block + 4*i);   // W[0..15]
    }

    /* Working variables a..h initialized from current H. part 6.2 */
    uint32_t a  = c -> h[0],
             b  = c -> h[1],
             c2 = c -> h[2],
             d  = c -> h[3],
             e  = c -> h[4],
             f  = c -> h[5],
             g  = c -> h[6],
             h  = c -> h[7];

    /* 64 rounds per part 6.2 using Σ0/Σ1, Ch, Maj, and K[t]. */
    for (int i = 0; i < 64; i++)
    {
        uint32_t W_i;
        if (i < 16)
        {
            W_i = W[i];
        }
        else
        {
            int temp = i & 15; // slot holding for W[i-16]
            uint32_t w15 = W[(i - 15) & 15];
            uint32_t w2 = W[(i - 2) & 15];
            W_i = s1(w2) + W[(i - 7) & 15] + s0(w15) + W[temp];
            W[temp] = W_i;
        }

        uint32_t T1 = h + S1(e) + Ch(e,f,g) + K[i] + W_i;
        uint32_t T2 = S0(a) + Maj(a,b,c2);
        h = g; 
        g = f; 
        f = e; 
        e = d + T1; 
        d = c2; 
        c2 = b; 
        b = a; 
        a = T1 + T2;
    }

    /* Add working vars back into state. part 6.2 */
    c -> h[0] += a; 
    c -> h[1] += b; 
    c -> h[2] += c2; 
    c -> h[3] += d;
    c -> h[4] += e; 
    c -> h[5] += f; 
    c -> h[6] += g; 
    c -> h[7] += h;
}

/* Initialize H to H(0). FIPS 180-4 part 5.3.3 */
void SHA256_Init(SHA256_ctx *c)
{
    for(int i = 0; i < 8; i++) 
    {
        c->h[i] = H0[i];
    }
    
    c->bits=0; c->buf_len=0;
}

/* Absorb arbitrary-length message. Block processing per part 6.2. */
void SHA256_Update(SHA256_ctx *c, const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t*)data;
    c->bits += (uint64_t)len * 8;            /* track total bits, part 5 */
    if(c->buf_len)
    {                          /* fill leftover buffer */
        size_t t = 64 - c->buf_len;
        if(t > len) 
        {
            t = len;
        }
        for(size_t i = 0; i < t; i++) 
        {
            c->buf[c->buf_len+i] = p[i];
        }
        
        c->buf_len += t; 
        p += t; 
        len -= t;
        
        if(c->buf_len == 64)
        { 
            compress_imp(c, c->buf); 
            c->buf_len=0; 
        }
    }
    
    while(len >= 64)
    { 
        compress_imp(c, p); 
        p += 64; 
        len -= 64; 
    }
    
    if(len)
    { 
        for(size_t i = 0; i < len; i++) 
        { 
            c->buf[i]=p[i]; 
        }

        c->buf_len = len; 
    }
}

/* Finalize: padding and length encoding per FIPS 180-4 part 5, then one last compress per part 6.2. */
void SHA256_Final(SHA256_ctx *c, uint8_t out[32])
{
    /* Append '1' bit then k zero bits so that length ≡ 448 mod 512. part 5 */
    c->buf[c->buf_len++] = 0x80;
    if (c->buf_len > 56)
    { 
        while(c->buf_len<64) 
        {
            c->buf[c->buf_len++]=0; 
        }

        compress_imp(c,c->buf); 
        c->buf_len=0; 
    }     
    
    while(c->buf_len<56) 
    {
        c->buf[c->buf_len++]=0;
    }

    /* Append 64-bit big-endian length of the message. part 5 */
    uint64_t bits = c->bits;
    for(int i = 7; i >= 0; i--) 
    {
        c->buf[ 56 + (7-i)] = (uint8_t)(bits >> (i*8));
    }

    compress_imp(c,c->buf);

    /* Output H as big-endian 32-bit words. part 6.2 */
    for(int i = 0; i < 8; i++) 
    {
        store_32(out + 4 * i, c -> h[i]);
    }

    /* Wipe state; use explicit_bzero/memset_s if available. */
    for(size_t i = 0; i < sizeof *c; i++) 
    {    
        ((volatile uint8_t*)c)[i]=0;

    }
}

int SHA256_Stream(FILE *fp, unsigned char out[32])
{
    SHA256_ctx c; 
    SHA256_Init(&c);
    unsigned char buf[1<<15];           // 32 KiB
    size_t n;
    
    while((n=fread(buf,1,sizeof buf,fp))>0)
    {
        SHA256_Update(&c, buf, n);
    }
    
    if (ferror(fp)) 
    { 
        return 0;
    }
    
    SHA256_Final(&c, out); 
    
    return 1;
}

/* ------------------------- Modified  Functions --------------------------------*/
// Wrapper 
void SHA256_Hash(const void* data, size_t len, uint8_t out[32])
{
    SHA256_ctx c;
    SHA256_Init(&c);
    SHA256_Update(&c, data, len);
    SHA256_Final(&c, out);
}

int SHA256_DHK_Stream(FILE *fp, const void *key, size_t key_len, uint8_t out[32]) 
{
    if (key_len == 0)
    {
        fprintf(stderr, "Key cannot be empty\n");
        return 0;                 // optional guard
    }
    uint8_t first_hash[32];

    if (!SHA256_Stream(fp, first_hash))
    {
        fprintf(stderr, "Error hashing file!\n");
        return 0;    // one pass over the file
    }

    SHA256_ctx c; 
    SHA256_Init(&c);
    SHA256_Update(&c, first_hash, sizeof first_hash);
    SHA256_Update(&c, key, key_len);
    SHA256_Final(&c, out);

    // wipe
    for (size_t i = 0; i < sizeof first_hash; i++) first_hash[i] = 0;

    return 1;
}
