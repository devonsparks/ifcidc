#ifndef _IFCIDC_H
#define _IFCIDC_H

#define IFCIDC_DECOM_LEN       (36)
#define IFCIDC_FIXED_DECOM_LEN (32)
#define IFCIDC_COM_LEN         (22)

#define BUFSIZE 80


typedef enum  {
        S_OK = 0,       
        S_ERR_INPUT_LEN,
        S_ERR_SENTINEL,
        S_ERR_ASCII,
        S_ERR_NORMALIZE,
        S_ERR_COM
} IFCIDC_Status;

extern IFCIDC_Status ifcidc_compress(const char *in, char *out);
extern IFCIDC_Status ifcidc_decompress(const char *in, char *out);
extern char *ifcidc_buffer_new();
extern void ifcidc_buffer_del(char *buf);

extern char *ifcidc_err_msg(IFCIDC_Status err);


#endif

