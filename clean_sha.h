#include <stddef.h>
#include <stdint.h>

#define SHA256_DIGEST_LENGTH 32  /* FIPS 180-4 part 6.2 */

typedef struct 
{
    uint32_t h[8];     /* H registers, FIPS 180-4 part 6.2 */
    uint64_t bits;     /* total message length in bits, part 5 */
    uint8_t  buf[64];  /* 512-bit block buffer, part 6.2 */
    size_t   buf_len;  /* bytes currently in buf */
} sha256_ctx;

void sha256_init(sha256_ctx *c);                                      /* part 6.2 */
void sha256_update(sha256_ctx *c, const void *data, size_t len);      /* part 6.2 */
void sha256_final(sha256_ctx *c, uint8_t out[SHA256_DIGEST_LENGTH]);  /* part 5, part 6.2 */
