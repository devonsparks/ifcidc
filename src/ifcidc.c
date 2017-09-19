#include <stdlib.h>
#include <assert.h>

#include <string.h>


#include "ifcidc.h"


#define B162I(A)  (b16mask[(unsigned char)A])
#define B642I(A)  (b64mask[(unsigned char)A])
#define IN_B16(A) (!(b16mask[(unsigned char)A] < 0))
#define IN_B64(A) (!(b64mask[(unsigned char)A] < 0))


static const char *
b64 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_$";


static const char *
b16 = "0123456789ABCDEF";


static const char
b16mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
              0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,          \
             -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} ;


static const char
b64mask[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
             -1, -1, -1, -1, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, \
              0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,          \
             -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
             25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, 62, \
             -1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, \
             51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1 };


static IFCIDC_Status
com(const char *in, char *out);


static IFCIDC_Status
decom(const char *in, char *out);

static IFCIDC_Status
fixid(const char *in, char *out);

static IFCIDC_Status
unfixid(const char *in, char *out);

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
  { S_ERR_COM,       "Unable to perform compression operation."}
};


static IFCIDC_Status
com(const char *in, char *out) {
  int i,oi, n;
 
  i = oi = n = 0;
  while(i < IFCIDC_FIXED_DECOM_LEN) {
    n  = B162I(in[i    ]) << 8;
    n += B162I(in[i + 1]) << 4;
    n += B162I(in[i + 2]);
    out[oi + 1] = b64[n % 64];
    out[oi    ] = b64[n / 64];
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
  while(i < IFCIDC_COM_LEN) {
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

char *
ifcidc_buffer_new() {
  assert(BUFSIZE > IFCIDC_DECOM_LEN);
  
  char *s = malloc((BUFSIZE) * sizeof(char));
  memset(s, ' ', (BUFSIZE) * sizeof(char));
  s[BUFSIZE - 1] = '\0';
  return s;
}

void
ifcidc_buffer_del(char *buf) {
  free(buf);
}

static IFCIDC_Status
fixid(const char *in, char *out) {
  unsigned int i, j;

  out[0] = '0';
  out[IFCIDC_FIXED_DECOM_LEN + 1] = '\0';
  
  for(i = j = 0; in[i] != '\0'; i++) {
    if(in[i] != '-') {
        out[++j] = in[i];
      }
  }

  assert(j == IFCIDC_FIXED_DECOM_LEN);

  return S_OK;
  
}

static IFCIDC_Status
unfixid(const char *in, char *out) {
  unsigned int i, j;

  out[IFCIDC_DECOM_LEN] = '\0';

  for(j = 0, i = 1; in[i] != '\0';) {
    if(j == 8 || j == 13 || j == 18 || j == 23) {
      out[j++] = '-';
    }
    else {       
      out[j++] = in[i++];
    }
  }

  return S_OK;
  
}

IFCIDC_Status
ifcidc_compress(const char *in, char *out) {
  char comed[IFCIDC_FIXED_DECOM_LEN + 1];
  unsigned char i;
  
    if(strlen(in) != IFCIDC_DECOM_LEN) {
      return S_ERR_INPUT_LEN;
      }

    if(in[IFCIDC_DECOM_LEN] != '\0') {
      return S_ERR_SENTINEL;
    }

    for(i = 0; in[i] != '\0'; i++) {
      if(in[i] != '-' && !IN_B16(in[i])) {
        return S_ERR_ASCII;
      }
    }

    if(fixid(in, comed) != S_OK) {
      return S_ERR_NORMALIZE;
    }


    if(com(comed, out) != S_OK) {
      return S_ERR_COM;
    }
    

  return S_OK;
}

IFCIDC_Status
ifcidc_decompress(const char *in, char *out) {
  char decomed[IFCIDC_FIXED_DECOM_LEN + 1];
  unsigned char i;

  if(strlen(in) != IFCIDC_COM_LEN) {
    return S_ERR_INPUT_LEN;
  }
     
  if(in[IFCIDC_COM_LEN] != '\0') {
    return S_ERR_SENTINEL;
  }

  for(i = 0; in[i] != '\0'; i++)
    if(!IN_B64(in[i]))
      return S_ERR_ASCII;
  
  
  if(decom(in, decomed) != S_OK) {
    return S_ERR_COM;
  }
  
  if(unfixid(decomed, out) != S_OK) {
    return S_ERR_NORMALIZE;
  }

  return S_OK;
}

char *
ifcidc_err_msg(IFCIDC_Status err) {
  unsigned short es;

  es = sizeof(errordesc)/sizeof(struct _errordesc);
  while(es-- > 0) {
    if (errordesc[es].code == err) {
      return errordesc[es].message;
    }
  }
  return "";
}


