#include "wavHeader.h"

_BOOL wavHeader::write_prelim_header (FILE * out, int channels, int samplerate)
{

  int knownlength = 0;

  unsigned int size = 0x7fffffff;
  // int channels = 2;
  // int samplerate = 44100;
  int bytespersec = channels * samplerate * bits / 8;
  int align = channels * bits / 8;
  int samplesize = bits;

  if (knownlength)
    size = (unsigned int) knownlength;

  memcpy (headbuf, "RIFF", 4);
  WRITE_U32 (headbuf + 4, size - 8);
  memcpy (headbuf + 8, "WAVE", 4);
  memcpy (headbuf + 12, "fmt ", 4);
  WRITE_U32 (headbuf + 16, 16);
  WRITE_U16 (headbuf + 20, 1);  /* format */
  WRITE_U16 (headbuf + 22, channels);
  WRITE_U32 (headbuf + 24, samplerate);
  WRITE_U32 (headbuf + 28, bytespersec);
  WRITE_U16 (headbuf + 32, align);
  WRITE_U16 (headbuf + 34, samplesize);
  memcpy (headbuf + 36, "data", 4);
  WRITE_U32 (headbuf + 40, size - 44);

  if (fwrite (headbuf, 1, 44, out) != 44) { return FALSE; }
  return TRUE;
}

_BOOL wavHeader::rewrite_header (FILE * out, unsigned int written)
{
  unsigned int length = written;

  length += 44;

  WRITE_U32 (headbuf + 4, length - 8);
  WRITE_U32 (headbuf + 40, length - 44);
  if (fseek (out, 0, SEEK_SET) != 0) {
    printf("%s\n", "Failed to seek on seekable wav file");
    return FALSE;
  }
  if (fwrite (headbuf, 1, 44, out) != 44) {
    printf("%s\n", "Failed to rewrite wav header");
    return FALSE;
  }
  return TRUE;
}
