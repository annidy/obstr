#include "obstr.h"
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#define MAXSIZE 10240

char *hexdump(uint8_t *hex, int len)
{
    static char ch[MAXSIZE*3];
    for (int i = 0; i < len; ++i)
    {
        sprintf(ch+i*5, "0x%02x,", (uint8_t)hex[i]);
    }
    ch[strlen(ch)-1] = 0;
    return ch;
}

char *get_var_str(char *org)
{
    static char name[MAXSIZE];
    int i, j;
    for (i = 0, j = 0; i < strlen(org) && i < sizeof(name)-1; i++) {
          if (strchr(" `~!@#$%%^&*()+-=\"'/.,<>|[]{}:;", org[i])) {
              name[j++] = '_';
          } else if (isprint((int)org[i])) {
              name[j++] = org[i];
          }
    }
    name[j++] = 0;
    return name;
}

unsigned short hash(char *str)
{
    unsigned short hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}



int main(int argc, char **argv)
{
    uint8_t out[MAXSIZE];
    char in[MAXSIZE];

    int i = 1;
    do {
        memset(in, MAXSIZE, 0);
        memset(out, MAXSIZE, 0);
        if (i == argc && i == 1) {
            char c;
            int  n = 0;
            while ((c = getchar()) != EOF) {
                in[n++] = c;
            }
        } else {
            strcpy(in, argv[i]);
        }
        int len = MAXSIZE;
        ObstrEnc(in, out, &len);
        *(out+len) = 0;

        printf("/* %s */\n"
               "static uint8_t obstr_%x[] = {%s};\n"
               "#define O_%s ObstrDec(obstr_%x)\n",
               in,
               hash(in), hexdump(out, len),
               get_var_str(in), hash(in));

        assert(strcmp(in, ObstrDec(out)) == 0);
        assert(strcmp(in, ObstrDec(out)) == 0);
    } while (++i < argc);
	return 0;
}