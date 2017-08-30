#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "ifcidc.h"

#define OK   0
#define NOK -1
#define B162I(B16A) (b16mask[B16A])
#define B642I(B64A) (b64mask[B64A])
#define IN_B16(A) ((A < 0x80) && b16mask[A] >= 0)
#define IN_B64(A) ((A < 0x80) && b64mask[A] >= 0)


static const char
b16mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


static const char
b64mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, 62, -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1};


static const char
*b64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$";

static const
char *b16 = "0123456789ABCDEF";

static char com(const char *in, char *out);
static char decom(const char *in, char *out);
static char fixid(const char *in, char **out);
static char unfixid(const char *in, char **out);

int
main(void)
{
   char *sample = "76cbdc2e-a238-4725-83a3-be115ba5459c";


  char *done;
  if(ifcidc_compress(sample, &done) == NOK) {
    printf("Error");
  return 0x80;
  }
  else {
    printf("%s\n", done);
  }
  
  char *dec;

    if(ifcidc_decompress(done, &dec) == NOK) {
    printf("Error");
    return 0x80;
  } else {
    printf("%s\n", dec);
    }

  free(done);
   free(dec);
  
  return 0;
}

/* fixid: strip out hypens, add zero pad */
static char
fixid(const char *in, char **out) {
  int i, j;
  char *s;

  if((s = malloc((1 + 32 + 1) * sizeof(char))) == NULL)
    return NOK;

  s[0] = '0';
  s[32 + 1] = '\0';
  
  for(i = j = 0; in[i] != '\0'; i++) {
    if(in[i] != '-') {
	s[++j] = in[i];
      }
  }

  assert(j == 32);

  *out = s;
  return OK;
  
}

/* unfixid: add hyphens in, remove zero pad */
static char
unfixid(const char *in, char **out) {
  int i, j;
  char *s;

  if((s = malloc((32 + 1) * sizeof(char))) == NULL)
    return NOK;

  s[32 + 1] = '\0';

  for(j = 0, i = 1; in[i] != '\0';) {
    if(j == 8 || j == 13 || j == 18 || j == 23) {
      s[j++] = '-';
    }
    else {       
      s[j++] = in[i++];
    }
  }

  *out = s;
  return OK;
  
}


char
ifcidc_compress(const char *in, char **out) {
  char *fixed;
  char *comed;
  short i;

  if(in[36] != '\0') {
    return NOK;
  }

  for(i = 0; in[i] != '\0'; i++) {
    if(in[i] != '-' && !IN_B16(in[i])) {
      return NOK;
    }
  }
  
  if(fixid(in, &fixed) == NOK) {
    return NOK;
  }
    
  if((comed = malloc((22 + 1) * sizeof(char))) == NULL) {
    return NOK;
  }
  
  if(com(fixed, comed) == NOK) {
    free(comed);
    return NOK;
  }

  free(fixed);
  *out = comed;
  return OK;
}

char
ifcidc_decompress(const char *in, char **out) {
  char *fixed;
  char *decomed;
  short i;

  if(in[22] != '\0') {
    return NOK;
  }

  for(i = 0; in[i] != '\0'; i++)
    if(!IN_B64(in[i]))
      return NOK;
  
  if((decomed = malloc((32 + 1) * sizeof(char))) == NULL) {
    return NOK;
  }
  
  if(decom(in, decomed) == NOK) {
    free(decomed);
    return NOK;
  }
  
  if(unfixid(decomed, &fixed) == NOK) {
    free(decomed);
    return NOK;
  }

  free(decomed);
  *out = fixed;
  return OK;
}


static char
com(const char *in, char *out) {
  int i,oi, n;
 
  i = oi = n = 0;
  while(i < 33) {
    n  = B162I(in[i    ]) << 8;
    n += B162I(in[i + 1]) << 4;
    n += B162I(in[i + 2]);
    out[oi + 1] = b64[n % 64];
    out[oi] = b64[n / 64];
    oi += 2;
    i  += 3;
  }
  out[oi] = '\0';
  return OK;
}

static char
decom(const char *in, char *out) {
  int i, oi, n, t;

  i = oi = n =  0;
  while(i < 22) {
    n  = B642I(in[i]) << 6;
    n += B642I(in[i + 1]);
    t  = n / 16;
    out[oi + 2] = b16[n % 16];
    out[oi + 1] = b16[t % 16];
    out[oi    ] = b16[t / 16];
    oi += 3;
    i  += 2;
  }
  out[oi] = '\0';
  return OK;
}
