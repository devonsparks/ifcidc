#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define B162I(B16A) ((B16A > '9')   ? (B16A &~ 0x20) - 'A' + 10 : (B16A - '0'))
#define B642I(B64A) ((B64A == '-')  ? 62 :                                  \
		     (B64A == '$')  ? 63 :				    \
		     ((B64A > '9')  ?				            \
		      ((B64A > 'Z') ?                                       \
		       (B64A & ~0x20) - 'A' + 36  :                         \
		       (B64A & ~0x20) - 'A' + 10) :	                    \
		      (B64A - '0')))

		      
const char *b64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$";
const char *b16 = "0123456789ABCDEF";


char compress(const char *in, char *out);
char decompress(const char *in, char *out);
static char
b16_cleanse(const char *in, unsigned short ic, char *out, unsigned short oc);
   

int
main(void)
{
  char *sample = "76cbdc2e-a238-4725-83a3-be115ba5459c";

  /* 32-bit UUID plus leading padding and sentinel = 34 bytes */
  char *s;
  int i, j;
  
  s = malloc((1 + 32 + 1) * sizeof(char));
  s[0] = '0';
  s[32 + 1] = '\0';

  i = j = 0;
  while(sample[i] != '\0') {
    if(sample[i] != '-')
      s[++j] = sample[i];
    i++;
  }


  char *out = malloc((22 + 1) * sizeof(char));
  char *uout = malloc((33 + 1) * sizeof(char));
  compress(s, out);
  decompress(out, uout);
  
  printf("%s\n", out);
  printf("\%s\n", uout);
  free(s);
  free(out);
  free(uout);

  return 0;
}

static char
b16_cleanse(const char *in, unsigned short ic, char *out, unsigned short oc) {
  assert(ic == 36);
  assert(oc == 33);
  out[oc] = '\0';
  ic--;
  while(ic > 0) {

    if(in[ic] != '-')
      out[--oc] = in[ic];
    printf("%d, %d, %c, %c\n", ic, oc, in[ic], out[oc]);
    ic--;
  }
  printf("%c\n", out[0]);
  return TRUE;
}

char
compress(const char *in, char *out) {
  int i,oi, n;
  
  i = oi = n = 0;
  while(i<33) {
    n  = B162I(in[i    ]) << 8;
    n += B162I(in[i + 1]) << 4;
    n += B162I(in[i + 2]);
    out[oi + 1] = b64[n%64];
    out[oi] = b64[n/64];
    oi += 2;
    i  += 3;
  }
  out[oi] = '\0';
  return TRUE;
}

char
decompress(const char *in, char *out) {
  int i, oi, n, t;

  i = oi = n =  0;
  while(i<22) {
    n  = B642I(in[i]) * 64;
    n += B642I(in[i + 1]);
    t = n>>4;
    out[oi + 2] = b16[n%16];
    out[oi + 1] = b16[t%16];
    out[oi    ] = b16[t/16];
    oi += 3;
    i  += 2;
  }
  out[oi] = '\0';
  return TRUE;
}

