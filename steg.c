/*
 * Author: Alex Adams
 * Many methods taken from Brooke Chenoweth,
 * see https://www.cs.unm.edu/~bchenoweth/cs241
 */


#include <stdio.h>
#include <stdlib.h>

void copyIntToAddress(int n, unsigned char bytes[])
{
  bytes[0] = n & 0xFF;
  bytes[1] = (n >>  8) & 0xFF;
  bytes[2] = (n >> 16) & 0xFF;
  bytes[3] = (n >> 24) & 0xFF;
}

int getIntFromArray(unsigned char bytes[])
{
  int n =
    bytes[0] |
    bytes[1] << 8 |
    bytes[2] << 16 |
    bytes[3] << 24;
  return n;
}

int main(int argc, char *argv[])
{
  char* infilename;
  char* outfilename;
  if (argc != 3)
  {
    printf("USAGE:\nsteg {input file} {output file}");
    exit(1);
  }
  infilename = argv[1];
  outfilename = argv[2];

  unsigned char header[54];

  FILE* in = fopen(infilename, "rb");
  FILE* out = fopen(outfilename, "wb");

  int pixelWidth;
  int pixelHeight;
  int rowSize;
  int rowPadding;

  int i, j;
  int copy = 0;

  /* read header into array */
  fread(header, 1, 54, in);

  /* is this really a bitmap? */
  if(header[0] != 'B' || header[1] != 'M')
  {
    printf("Input file is not a bitmap\n");
    exit(1);
  }

  /* Is the header size what we expect? */
  if(getIntFromArray(&header[10]) != 54)
  {
    printf("Unexpected header size\n");
    exit(1);
  }

  /* How many bits per pixel? Expecting 24*/
  if(!(header[28] == 24 && header[29] == 0))
  {
    printf("Unexpected number of bits/pixel\n");
    exit(1);
  }

  pixelWidth = getIntFromArray(&header[18]);
  pixelHeight = getIntFromArray(&header[22]);

  /* compute row padding */
  rowSize = pixelWidth*3;
  rowPadding = (4 - (rowSize % 4)) % 4;
  rowSize += rowPadding;

  /* write header to output file */
  fwrite(header, 1, sizeof(header), out);

  /* Read RGB data from original, copy with message*/
  for(i = 0; i < pixelHeight; ++i)
  {
    for(j = 0; j < pixelWidth; j++)
    {
      unsigned char bytes[4];
      unsigned int message;

      message = getchar();

      /* color order is BGR */
      fread(bytes, 1, 4, in);

      /* set last bytes to message */
      if(!copy){
        if(message == EOF)
        {
          bytes[0] &= 0xFC;
          bytes[1] &= 0xFC;
          bytes[2] &= 0xFC;
          bytes[3] &= 0xFC;
          copy = 1;
        }
        else
        {
          bytes[3]=(message & 3) | (bytes[3] & 0xFC);
          bytes[2]=(message >> 2) | (bytes[2] & 0xFC);
          bytes[1]=(message >> 4) | (bytes[1] & 0xFC);
          bytes[0]=(message >> 6) | (bytes[0] & 0xFC);
        }
      }
      fwrite(bytes, 1, 4, out);
    }
  }

  fclose(in);
  fclose(out);
  return 0;
}