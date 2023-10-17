/*==============================================================================
  
   Purpose:    functions on TIFF data
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include <tiffio.h>            /* link with -ltiff */

#include "tiff.h"

/*============================================================================*/
/* read a TIFF file into a RGB image */

int TiffReadToRGB(char *filename, int *height, int *width, rgb_t **image) {

  TIFF *tiff;
  size_t npixels;


  /* open TIFF file */
  if((tiff = TIFFOpen(filename, "r")) == NULL)
    {
      printf("problems opening file %s\n", filename);
      return -1;
    }

  /* read RGB data */
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, width);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, height);
  npixels = *width * *height;
  *image = (rgb_t *) malloc(npixels * sizeof (**image));
  if (*image == NULL) 
    return -1;

  if (TIFFReadRGBAImage(tiff, *width, *height, (uint32 *)*image, 0) == 0) {
    free(*image);
    return -1;
  }

  /* close TIFF file */
  TIFFClose(tiff);

  /* everything ok */
  return 0;
}

/*============================================================================*/
/* write a TIFF file from a black and white image */

int TiffWriteFromGray(char *filename, int height, int width, gray_t image[height][width])
{
  TIFF *tiff;

  /* open TIFF file */
  if((tiff = TIFFOpen(filename, "w")) == NULL)
    return -1;

  // Write the tiff tags to the file
  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);

  TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);

  // transpose the image
  gray_t t_image[height][width];

  for(int i=0; i<height; i++)
      for(int j=0; j<width; j++)
	t_image[i][j] = image[height-i-1][j];

  // Actually write the image
  if(TIFFWriteEncodedStrip(tiff, 0, t_image, width * height * 1) == 0)
    return -1;

  TIFFClose(tiff);
  return 0;
}

/*============================================================================*/
/* write a TIFF file from a black and white image */

/*??? Fehler */

int TiffWriteFromBW(char *filename, int height, int width, blackwhite_t image[height][width])
{
  TIFF *tiff;


  /* open TIFF file */
  if((tiff = TIFFOpen(filename, "w")) == NULL)
    return -1;

  // Write the tiff tags to the file
  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);

  TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);

  // transpose the image
  blackwhite_t t_image[height][width];

  for(int i=0; i<height; i++)
      for(int j=0; j<width; j++)
	t_image[i][j] = image[height-i-1][j];

  // Actually write the image
  if(TIFFWriteEncodedStrip(tiff, 0, t_image, width * height * 1) == 0)
    return -1;

  TIFFClose(tiff);
  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
