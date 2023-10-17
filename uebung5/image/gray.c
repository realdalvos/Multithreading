/*==============================================================================
  
   Purpose:    functions on gray images
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include "commondefs.h"
#include "rgb.h"
#include "gray.h"

#include <libFHBRS.h>

#include <stdlib.h>


//==============================================================================
// convert a RGB image to a gray image

int
RGBtoGray (int height,		       // I: height of image
	   int width,		       // I: width of image
	   rgb_t rgb_image[height][width],	// I: RGB image
	   gray_t gray_image[height][width]	// O: gray image
	)
{
  // convert pixels
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; ++j)
      {
	// get image element
	rgb_t val = rgb_image[i][j];

	/* convert RGB to gray */
	rgb_color_t red = RGB_RED_PART (val);
	rgb_color_t green = RGB_GREEN_PART (val);
	rgb_color_t blue = RGB_BLUE_PART (val);
	int cval = (int) ((54 * red + 183 * green + 19 * blue) / 256.0);
	gray_t gval = min (max (cval, GRAY_MIN), GRAY_MAX);

	// convert RGB to gray
	gray_image[i][j] = gval;
      }

  // everything ok
  return 0;
}

//==============================================================================
// convert a RGB image to a gray image

int
BWtoGray (int height,		       // I: height of image
	  int width,		       // I: width of image
	  blackwhite_t bw_image[height][width],	// I: black/white image
	  gray_t gray_image[height][width]	// O: gray image
	)
{
  // convert pixels
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; ++j)
      {
	// get image element
	blackwhite_t val = bw_image[i][j];

	// convert bw to gray
	gray_image[i][j] = (val == 0) ? GRAY_MIN : GRAY_MAX;
      }

  // everything ok
  return 0;
}

//==============================================================================
// invert gray image

int
GrayInvert (int height,		       // I: height of image
	    int width,		       // I: width of image
	    gray_t image[height][width]	// I/O: gray image
	)
{
  // convert pixels
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; ++j)
      {
	// invert pixel
	image[i][j] = GRAY_MAX - image[i][j];
      }

  // everything ok
  return 0;
}

//==============================================================================
// contrast stretching: range of gray values is extended

int
GrayContrastStretching (int height,    // I: height of image
			int width,     // I: width of image
			gray_t image[height][width]	// I/O: gray image
	)
{
  // determine minimum and maximum gray values in image
  gray_t minval = GRAY_MAX;
  gray_t maxval = GRAY_MIN;

  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      {
	if (image[i][j] < minval)
	  minval = image[i][j];
	if (image[i][j] > maxval)
	  maxval = image[i][j];
      }

  /* scale gray values:
   * xl, xh: old minimum/maximum values
   * xL, xH: desired minimum/maximum values
   * xi = (xi-xl)*((xH-xL)/xh-xl))+xL
   */
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      {
	image[i][j] = (image[i][j] - minval) * ((GRAY_MAX - GRAY_MIN) / (maxval - minval)) + GRAY_MIN;
      }

  return 0;
}

//==============================================================================
// mean of 3x3 neighborhood

int
GrayMean_3x3 (int height,	       // I: height of image
	      int width,	       // I: width of image
	      gray_t image[height][width],	// I: gray image
	      gray_t new_image[height][width]	// O: result gray image
	)
{
  for (int i = 1; i < height - 1; i++)
    for (int j = 1; j < width - 1; j++)
      {
	unsigned int val =
		image[i - 1][j - 1]
		+ image[i - 1][j]
		+ image[i - 1][j + 1] + image[i][j - 1] + image[i][j] + image[i][j + 1] + image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1];

	new_image[i][j] = (gray_t) (val / 9);
      }

  // everything ok
  return 0;
}

//===============================================================================
// median of 3x3 neighborhood

static int
gray_compare (const void *p1, const void *p2)
{
  return *(gray_t *) p2 - *(gray_t *) p1;
}

int
GrayMedian_3x3 (int height,	       // I: height of image
		int width,	       // I: width of image
		gray_t image[height][width],	// I: gray image
		gray_t new_image[height][width]	// O: result gray image
	)
{
  gray_t a[9];


  for (int i = 1; i < height - 1; i++)
    for (int j = 1; j < width - 1; j++)
      {

	// get neighborhood
	a[0] = image[i - 1][j - 1];
	a[1] = image[i - 1][j];
	a[2] = image[i - 1][j + 1];
	a[3] = image[i][j - 1];
	a[4] = image[i][j];
	a[5] = image[i][j + 1];
	a[6] = image[i + 1][j - 1];
	a[7] = image[i + 1][j];
	a[8] = image[i + 1][j + 1];

	// sort elements
	qsort (a, sizeof (a[0]), 9, gray_compare);

	// store median
	new_image[i][j] = a[4];
      }

  // everything ok
  return 0;
}

//==============================================================================
// median of 5x5 neighborhood

int
GrayMedian_5x5 (int height,	       // I: height of image
		int width,	       // I: width of image
		gray_t image[height][width],	// I: gray image
		gray_t new_image[height][width]	// O: result gray image
	)
{
  gray_t a[25];


  for (int i = 2; i < height - 2; i++)
    for (int j = 2; j < width - 2; j++)
      {

	// get neighborhood
	int i2 = 0;
	for (int i1 = -2; i1 <= 2; i1++)
	  for (int j1 = -2; j1 <= 2; j1++)
	    a[i2++] = image[i + i1][j + j1];

	// sort elements
	qsort (a, sizeof (a[0]), 25, gray_compare);

	// store median
	new_image[i][j] = a[12];
      }

  // everything ok
  return 0;
}

//==============================================================================
// display a gray image

int
GrayDisplay (int height,	       // I: height of image
	     int width,		       // I: width of image
	     gray_t image[height][width]	// I: gray image
	)
{
  char c;
  int window_number;

  if ((window_number = graphic_start (width, height, "gray image")) < 0)
    return -1;

  for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
	{
	  graphic_setGrayColor (window_number, image[height - i - 1][j]);
	  graphic_drawPoint (window_number, j, i);
	}
      graphic_flush (window_number);
    }

#if 1
  printf ("hit any key ");
  scanf ("%c", &c);
  graphic_end (window_number);
#endif

  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
