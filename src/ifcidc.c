#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "ifcidc.h"


#define B162I(A)  (b16mask[(unsigned char)A])
#define B642I(A)  (b64mask[(unsigned char)A])
#define IN_B16(A) (!(b16mask[(unsigned char)A] < 0))
#define IN_B64(A) (!(b64mask[(unsigned char)A] < 0))

#define COMLEN    (22)
#define DECOMLEN  (32)
  
static const char
b16mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


static const char
b64mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, 62, -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1};


static const char *
b64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$";


static const char *
b16 = "0123456789ABCDEF";

static const struct
_errordesc {
  int  code;
  char *message;
} errordesc[] = {
  { S_OK,            "Compression successful." },
  { S_ERR_INPUT_LEN, "Unexpected input length."},
  { S_ERR_SENTINEL,  "Expected string sentinel not found."},
  { S_ERR_ASCII,     "Non-ASCII character found in input."},
  { S_ERR_NORMALIZE, "Unable to normalize input string."},
  { S_ERR_ALLOC,     "Unable to allocate space for processing."},
  { S_ERR_COM,       "Unable to perform compression operation."}
};

static IFCIDC_Status
com(const char *in, char *out);


static IFCIDC_Status
decom(const char *in, char *out);


static IFCIDC_Status
fixid(const char *in, char **out);


static IFCIDC_Status
unfixid(const char *in, char **out);



char *
ifcidc_err_msg(IFCIDC_Status err) {
  short es;

  es = sizeof(errordesc)/sizeof(struct _errordesc);
  while(es-- > 0) {
    if (errordesc[es].code == err) {
      return errordesc[es].message;
    }
  }
  return "";
}
  
  
IFCIDC_Status
ifcidc_compress(const char *in, char **out) {
  char *fixed;
  char *comed;
  unsigned char i;

  if(strlen(in) != (DECOMLEN + 3 + 1)) {
    return S_ERR_INPUT_LEN;
    }
    
  if(in[DECOMLEN + 3 + 1] != '\0') {
    return S_ERR_SENTINEL;
  }

  for(i = 0; in[i] != '\0'; i++) {
    if(in[i] != '-' && !IN_B16(in[i])) {
      return S_ERR_ASCII;
    }
  }
  
  if(fixid(in, &fixed) != S_OK) {
    return S_ERR_NORMALIZE;
  }
    
  if((comed = malloc((COMLEN + 1) * sizeof(char))) == NULL) {
    return S_ERR_ALLOC;
  }
  
  if(com(fixed, comed) != S_OK) {
    free(comed);
    return S_ERR_COM;
  }

  free(fixed);  
  *out = comed;
  return S_OK;
}



IFCIDC_Status
ifcidc_decompress(const char *in, char **out) {
  char *fixed;
  char *decomed;
  unsigned char i;

  if(strlen(in) != COMLEN) {
    return S_ERR_INPUT_LEN;
  }
     
  if(in[COMLEN] != '\0') {
    return S_ERR_SENTINEL;
  }

  for(i = 0; in[i] != '\0'; i++)
    if(!IN_B64(in[i]))
      return S_ERR_ASCII;
  
  if((decomed = malloc((1 + DECOMLEN)* sizeof(char))) == NULL) {
    return S_ERR_ALLOC;
  }
  
  if(decom(in, decomed) != S_OK) {
    free(decomed);
    return S_ERR_COM;
  }
  
  if(unfixid(decomed, &fixed) != S_OK) {
    free(decomed);
    return S_ERR_NORMALIZE;
  }

  free(decomed);
  *out = fixed;
  return S_OK;
}



/* fixid: strip out hypens, add zero pad */
static IFCIDC_Status
fixid(const char *in, char **out) {
  unsigned int i, j;
  char *s;

  if((s = malloc((1 + DECOMLEN + 1) * sizeof(char))) == NULL)
    return S_ERR_ALLOC;

  s[0] = '0';
  s[DECOMLEN + 1] = '\0';
  
  for(i = j = 0; in[i] != '\0'; i++) {
    if(in[i] != '-') {
	s[++j] = in[i];
      }
  }

  assert(j == DECOMLEN);

  *out = s;
  return S_OK;
  
}



/* unfixid: add hyphens in, remove zero pad */
static IFCIDC_Status
unfixid(const char *in, char **out) {
  unsigned int i, j;
  char *s;

  if((s = malloc((32 + 1) * sizeof(char))) == NULL)
    return S_ERR_ALLOC;

  s[DECOMLEN] = '\0';

  for(j = 0, i = 1; in[i] != '\0';) {
    if(j == 8 || j == 13 || j == 18 || j == 23) {
      s[j++] = '-';
    }
    else {       
      s[j++] = in[i++];
    }
  }

  *out = s;
  return S_OK;
  
}



static IFCIDC_Status
com(const char *in, char *out) {
  int i,oi, n;
 
  i = oi = n = 0;
  while(i < DECOMLEN) {
    n  = B162I(in[i    ]) << 8;
    n += B162I(in[i + 1]) << 4;
    n += B162I(in[i + 2]);
    out[oi + 1] = b64[n % 64];
    out[oi] = b64[n / 64];
    oi += 2;
    i  += 3;
  }
  out[oi] = '\0';
  return S_OK;
}



static IFCIDC_Status
decom(const char *in, char *out) {
  int i, oi, n, t;

  i = oi = n =  0;
  while(i < COMLEN) { // check stop condition
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
  return S_OK;
}

