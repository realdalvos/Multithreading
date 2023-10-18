/*==============================================================================
  
   Purpose:    functions on black and white data
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
	       53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de
  
==============================================================================*/

#include "commondefs.h"
#include "rgb.h"
#include "blackwhite.h"

#include <libFHBRS.h>
#include <stdlib.h>

//==============================================================================
// convert a RGB image to black and white image

int
RGBtoBw (int height,		       // I: height of image
	 int width,		       // I: width of image
	 rgb_t rgb_image[height][width],	// I: RGB image
	 int treshhold,		       // I: threshhold (-1:don't supplied)
	 blackwhite_t bw_image[height][width]	// O: b/w image
	)
{
  // convert to gray image
  gray_t gray_image[height][width];
  RGBtoGray (height, width, rgb_image, gray_image);

  // convert to bw
  graytoBw (height, width, gray_image, treshhold, bw_image);

  return 0;
}

//==============================================================================
// convert a gray image to black and white image

static int
bw_compare (const void *p1, const void *p2)
{
  return *(blackwhite_t *) p2 - *(blackwhite_t *) p1;
}


int
graytoBw (int height,		       // I: height of image
	  int width,		       // I: width of image
	  gray_t gray_image[height][width],	// I: gray image
	  int treshhold,	       // I: threshhold (-1:don't supplied)
	  blackwhite_t bw_image[height][width]	// O: b/w image
	)
{
  // take median
  if (treshhold == -1)
    {
      // histogram
      unsigned int hist[GRAY_MAX];

      for (int i = 0; i < GRAY_MAX; i++)
	hist[i] = 0;

      for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++)
	  hist[gray_image[i][j]]++;


      // sort histogram
      qsort (hist, GRAY_MAX, sizeof (hist[0]), bw_compare);

      treshhold = hist[GRAY_MAX / 2];
    }

  // convert pixels
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      {
	/* convert gray to black and white */
	bw_image[i][j] = (gray_image[i][j] < treshhold);
      }

  return 0;
}

//==============================================================================
// invert a black and white image

int
BWInvert (int height,		       // I: height of image
	  int width,		       // I: width of image
	  blackwhite_t bw_image1[height][width],	// I: b/w image
	  blackwhite_t bw_image2[height][width]	// O: b/w image
	)
{
  // convert pixels
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      {
	// invert pixel
	bw_image2[i][j] = ~bw_image1[i][j];
      }

  return 0;
}

//==============================================================================
// display a black and white image */

int
BWDisplay (int height,		       // I: height of image
	   int width,		       // I: width of image
	   blackwhite_t image[height][width]	// I: b/w image
	)
{
  char c;

  int window_number = graphic_start (width, height, "black and white image");
  for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
	{
	  graphic_setColor (window_number, (image[height - i - 1][j]) ? GRAPHIC_BLACK : GRAPHIC_WHITE);
	  graphic_drawPoint (window_number, j, i);
	}
    }
  graphic_flush (window_number);

  printf ("hit any key ");
  scanf ("%c", &c);
  graphic_end (window_number);

  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
