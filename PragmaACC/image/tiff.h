/*==============================================================================
  
   Purpose:    functions on TIFF data
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#if !defined(TIFF_H_INCLUDED)
#define TIFF_H_INCLUDED

#include "blackwhite.h"
#include "rgb.h"

/*============================================================================*/
/* functions */

/* read a TIFF file into a RGB image */
extern int TiffReadToRGB(char *filename, int *height, int *width, rgb_t **raster);
extern int TiffWriteFromGray(char *filename, int height, int width, gray_t image[height][width]);
extern int TiffWriteFromBW(char *filename, int height, int width, blackwhite_t image[height][width]);

#endif

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
