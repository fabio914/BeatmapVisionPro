#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include "bool.h"

class wavHeader {
private:
#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);


  int quiet = 0;
  int bits = 16;
  int endian = 0;
  int raw = 0;
  int sign = 1;
  unsigned char headbuf[44];  /* The whole buffer */
public:
    _BOOL write_prelim_header (FILE * out, int channels, int samplerate);
    _BOOL rewrite_header (FILE * out, unsigned int written);
};
