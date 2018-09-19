
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ifcidc.h"
static IFCIDC_Status
process_lines(FILE *fip,
	      FILE *fop,
	      const unsigned short si,
	      const unsigned short so,
	      IFCIDC_Status (*processor)(const char *in, char *out));
int
main(const int argc, char *argv[])
{

  char *fin;
  char *fon;
  FILE *fip;
  FILE *fop;
  int opt;
  unsigned short com;
  IFCIDC_Status status; 

  com = 1;
  fin = NULL;
  fon = NULL;
  fip = stdin;
  fop = stdout;
  while ((opt = getopt(argc, argv, "cxi:o:")) != -1) {
    switch(opt) {
    case 'c':
      com = 1;
      break;
    case 'x':
      com = 0;
      break;
    case 'i':
      fin = optarg;
      break;
    case 'o':
      fon = optarg;
      break;
    default:    
      break;
    }
  }


  if(fin != NULL) {
    if((fip = fopen(fin, "r")) == NULL) {
      fprintf(stderr,"Failed to open file %s\n", fin);
      return EXIT_FAILURE;
    }    
  }


  if(fon != NULL) {
    if((fop = fopen(fon, "w")) == NULL) {
      fprintf(stderr,"Failed to open file %s\n", fon);
      return EXIT_FAILURE;
    }    
  }

  status = (com == 1) ?
    process_lines(fip, fop, IFCIDC_DECOM_LEN, IFCIDC_COM_LEN,   &ifcidc_compress)   :
    process_lines(fip, fop, IFCIDC_COM_LEN,   IFCIDC_DECOM_LEN, &ifcidc_decompress) ;

  fclose(fip);
  fclose(fop);

  if(status != S_OK) {
    fprintf(stderr, "%s: %s\n", argv[0], ifcidc_err_msg(status));
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;

}
static IFCIDC_Status
process_lines(FILE *fip,
	      FILE *fop,
	      const unsigned short si,
	      const unsigned short so,
	       IFCIDC_Status (*processor)(const char *in, char *out)) {

    IFCIDC_Status s;
    char *in, *out;

    if((s = ifcidc_buffer_new(&in)) != S_OK)
      return s;
    if((s = ifcidc_buffer_new(&out)) != S_OK)
      return s;
    while (fgets(in, BUFSIZE, fip) != NULL) {
      in[si] = '\0';
      if((s = processor(in, out)) != S_OK) {
         ifcidc_buffer_del(in);
         ifcidc_buffer_del(out);
         return s;
      }
      else {
         fprintf(fop, "%s\n", out);
      } 
    }

    ifcidc_buffer_del(in);
    ifcidc_buffer_del(out);
    return S_OK;
}
