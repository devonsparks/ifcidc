#ifndef _IFCIDC_H
#define _IFCIDC_H

#define IFCIDC_OK   ((char)0)
#define IFCIDC_NOK ((char)-1)

#define IFCIDC_DECOM_LEN (36)
#define IFCIDC_FIXED_DECOM_LEN (32)
#define IFCIDC_COM_LEN   (22)

typedef enum  {
  S_OK = 0,
  S_ERR_INPUT_LEN = 1,
  S_ERR_SENTINEL = 2,
  S_ERR_ASCII = 3,
  S_ERR_NORMALIZE = 4,
  S_ERR_ALLOC = 5,
  S_ERR_COM = 6,
  S_ERR_UNKNOWN = 7
} IFCIDC_Status;


extern IFCIDC_Status ifcidc_compress(const char *in, char **out);
extern IFCIDC_Status ifcidc_decompress(const char *in, char **out);
extern char *ifcidc_err_msg(IFCIDC_Status err);
#endif
