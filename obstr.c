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
    int vs = (strlen(v) + 7) & 0xfffffff8;
    int leb = vs;
    int osize = 0;
    // LEB128 codec.
    while (leb > 127) {
        *out++ = leb | 0x80;
        leb >>= 7;
        osize++;
    }
    osize++;
    *out++ = leb;

    memcpy(out, v, strlen(v)+1);
    for (int i = 0; i < vs/8; ++i) {
        encipher(16, (uint32_t *)out + 2 * i, s_obstrkey);
    }
    *outsize = osize + vs;
}

char *ObstrDec(uint8_t *v)
{
    if (*v) {
        int vs = 0;
        uint8_t *bv = v;
        uint8_t byte = *v++;
        if (byte < 128) {
            vs = byte;
        } else {
            vs = byte & 0x7f;
            unsigned shift = 7;
            do
            {
                byte = *v++;
                vs |= (byte & 0x7f) << shift;
                shift += 7;
            } while (byte >= 128);
        }
        while (bv != v) {
            *bv++ = 0;
        }

        for (int i = 0; i < vs / 8; ++i)
        {
            decipher(16, (uint32_t*)v+2*i, s_obstrkey);
        }
    } else {
        while (!*(++v)) {}
    }
    return (char *)v;
}
