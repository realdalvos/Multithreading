/*==============================================================================
  
   Purpose:    functions on RGB images
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include <libFHBRS.h>          /* link with -lFHBRS -lX11 */

#include "commondefs.h"
#include "rgb.h"
#include "gray.h"


/*============================================================================*/
/* convert a gray image to a RGB image */

int GraytoRGB(int height, int width, gray_t gray_image[height][width],
	      rgb_t rgb_image[height][width])
{
  /* convert pixels */
  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++)
      {
	/* get image element */
	rgb_t val = gray_image[i][j];
	
	/* convert gray to RGB */
	rgb_image[i][j] = RGB_VAL(val, val, val);
      }

  /* everything ok */
  return 0;
}

/*============================================================================*/
/* display a RGB image */

int RGBDisplay(int height, int width, rgb_t image[height][width])
{
  char c;
  int window_number;

  if((window_number = graphic_start(width, height, "RGB image")) < 0)
    return -1;

  for(int i=0; i<height; i++)
      for(int j=0; j<width; j++)
	{
	  rgb_t rgb_color = image[height-i-1][j];
	  graphic_setRGBColor(window_number, RGB_RED_PART(rgb_color),
			      RGB_GREEN_PART(rgb_color),
			      RGB_BLUE_PART(rgb_color));
	  graphic_drawPoint(window_number, j,i);
	}

  graphic_flush(window_number);
  printf("hit any key ");
  scanf("%c", &c);

  graphic_end(window_number);

  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
