#include "obstr.h"
#include <stdio.h>

/* hello */
static uint8_t obstr_3099[] = {0x7b,0xe4,0xf4,0xf7,0xca,0x9d,0xc3,0x69,0x36,0xed,0xff,0xac};
#define O_hello ObstrDec(obstr_3099)
/* aaaaaaaa */
static uint8_t obstr_808d[] = {0xb0,0x03,0x4a,0x4a,0x01,0x7a,0x7d,0xd4,0x8d,0xc2,0x9a,0x36};
#define O_aaaaaaaa ObstrDec(obstr_808d)


char *hexdump(uint8_t *hex, int len)
{
    static char ch[1024];
    for (int i = 0; i < len; ++i)
    {
        sprintf(ch+i*5, "0x%02x,", (uint8_t)hex[i]);
    }
    ch[strlen(ch)-1] = 0;
    return ch;
}

char *get_var_str(char *org)
{
    static char name[1024];
    int i;
    for (i = 0; i < strlen(org); i++) {
          if (strchr(" `~!@#$%%^&*()+-=\"'/.,<>|[]{}:;", org[i])) {
              name[i] = '_';
          } else {
              name[i] = org[i];
          }
    }
    name[i] = 0;
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
  uint8_t out[1024] = {0};
	for (int i = 1; i < argc; i++) {
      char *str = argv[i];
  		int len = 1024;
	   	ObstrEnc(str, out, &len);

        printf("/* %s */\n"
               "static uint8_t obstr_%x[] = {%s};\n"
               "#define O_%s ObstrDec(obstr_%x)\n",
               str,
               hash(str), hexdump(out, len),
               get_var_str(str), hash(str));
	}
	if (argc == 1) {

		printf("%s %s\n", O_hello, O_aaaaaaaa);
	}
	return 0;
}