#ifndef OBSTR_H
#define OBSTR_H
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif 

void ObstrEnc(const char *v, uint8_t *out, int *outsize);
char *ObstrDec(uint8_t *v);

#ifdef __cplusplus
}
#endif

#endif