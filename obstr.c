#include "obstr.h"
 
// https://en.wikipedia.org/wiki/XTEA
#define DELTA 0x9E3779B9 

/* take 64 bits of data in v[0] and v[1] and 128 bits of key[0] - key[3] */
static void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=DELTA;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=DELTA, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}

static uint32_t s_obstrkey[] = {0x0, 0x0, 0x0, 0x0};

void ObstrEnc(const char *v, uint8_t *out, int *outsize)
{
    int vs = (strlen(v)+7) & 0xfffffff8;
    *outsize = vs+4;
    if (!out) return;
    uint32_t *iout = (uint32_t *)out+1;
    memcpy(iout, v, strlen(v)+1);
    for (int i = 0; i < vs/8; ++i) {
        encipher(16, iout+2*i, s_obstrkey);
    }
    *(uint32_t *)out = vs ^ *iout ^ DELTA;
}

char *ObstrDec(uint8_t *v)
{
    uint32_t vs = *(uint32_t *)v;
    uint32_t *vt = (uint32_t *)v+1;
    if (vs != 0) {
        vs = vs ^ *vt ^ DELTA;
        for (int i = 0; i < vs/8; ++i) {
            decipher(16, vt+2*i, s_obstrkey);
        }
        *(uint32_t *)v = 0;
    }
    return (char *)vt;
}
