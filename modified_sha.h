#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define SHA256_DIGEST_LENGTH 32  /* FIPS 180-4 part 6.2 */

typedef struct 
{
    uint32_t h[8];     /* H registers, FIPS 180-4 part 6.2 */
    uint64_t bits;     /* total message length in bits, part 5 */
    uint8_t  buf[64];  /* 512-bit block buffer, part 6.2 */
    size_t   buf_len;  /* bytes currently in buf */
} SHA256_ctx;

void SHA256_Init(SHA256_ctx *c);                                      /* part 6.2 */
void SHA256_Update(SHA256_ctx *c, const void *data, size_t len);      /* part 6.2 */
void SHA256_Final(SHA256_ctx *c, uint8_t out[SHA256_DIGEST_LENGTH]);  /* part 5, part 6.2 */
// Handle File:
int SHA256_Stream(FILE *fp, unsigned char out[32]);


void SHA256_Hash(const void* data, size_t len, uint8_t out[32]);
void SHA256_DHK(const void *key, size_t key_len, const void *msg, 
                size_t message_len, uint8_t out[32]);
int SHA256_DHK_Stream(FILE *fp, const void *key, size_t key_len, uint8_t out[32]);
// New Modified file:
void compress_imp(SHA256_ctx *c, const uint8_t block[64]);