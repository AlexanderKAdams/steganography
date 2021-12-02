//
// Author: Alex Adams
// Many methods taken from Brooke Chenoweth,
// see https://www.cs.unm.edu/~bchenoweth/cs241
//

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
  if (argc != 3)
  {
    printf("USAGE:\nsteg {input file} {output file}");
    exit(1);
  }
  char* infilename = argv[1];
  char* outfilename = argv[2];

  unsigned char header[54];

  FILE* in = fopen(infilename, "rb");
  FILE* out = fopen(outfilename, "wb");

  int fileSize;
  int pixelWidth;
  int pixelHeight;
  int pixelDataSize;
  int rowSize;
  int rowPadding;

  int i, j;

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

  fileSize = getIntFromArray(&header[2]);
  pixelWidth = getIntFromArray(&header[18]);
  pixelHeight = getIntFromArray(&header[22]);
  pixelDataSize = getIntFromArray(&header[34]);

  /* compute row padding */
  rowSize = pixelWidth*3;
  rowPadding = (4 - (rowSize % 4)) % 4;
  rowSize += rowPadding;

  printf("pixelWidth  = %d pixels\n", pixelWidth);
  printf("pixelHeight = %d pixels\n", pixelHeight);
  printf("rowPadding  = %d bytes\n", rowPadding);
  printf("rowSize     = %d bytes\n", rowSize);
  printf("pixelDataSize = %d bytes\n", pixelDataSize);
  printf("fileSize = %d bytes\n", fileSize);

  /* write header to output file */
  fwrite(header, 1, sizeof(header), out);

  /* Read RGB data from original, copy with message*/
  for(i = 0; i < pixelHeight; ++i)
  {
    for(j = 0; j < pixelWidth; ++j)
    {
      unsigned char bytes[8];
      unsigned char message;
      int k;

      /* color order is BGR */
      fread(bytes, 1, 8, in);

      /* set last bytes to message */
      for (k=0; k<2; k++)
      {
        message = getchar();
        if (message != '\0')
        {
          bytes[6+k] = message;
        }
      }

      fwrite(bytes, 1, 8, out);
    }

    /* handle end of row padding */
    fseek(in, rowPadding, SEEK_CUR);
    for(j = 0; j < rowPadding; ++j)
    {
      putc(0, out);
    }
  }

  fclose(in);
  fclose(out);
  return 0;
}