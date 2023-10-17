/*==============================================================================
  
   Purpose:    RGB functions
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#if !defined(RGB_H_INCLUDED)
#define RGB_H_INCLUDED

#include <tiffio.h>            /* link with -ltiff */

/*============================================================================*/
/* makros: */

/* RGB values are packed into one int as three 8 Bit values */
#define RGB_RED_PART(val)     ((rgb_color_t)((val) & 0xff))
#define RGB_GREEN_PART(val)   ((rgb_color_t)(((val) >> 8) & 0xff))
#define RGB_BLUE_PART(val)    ((rgb_color_t)(((val) >> 16) & 0xff))

/* RGB values are packed into one int as three 8 Bit values */
#define RGB_VAL(red, green, blue) (  ((rgb_color_t)((red)    & 0xff)) \
                                   | ((rgb_color_t)(((green) & 0xff) << 8)) \
				   | ((rgb_color_t)(((blue)  & 0xff) << 16)) )

/*============================================================================*/
/* types: */

typedef unsigned char rgb_color_t;
typedef unsigned int rgb_t;

/*============================================================================*/
/* functions: */

#include "gray.h"

/* convert a gray image to a RGB image */
int GraytoRGB(int height, int width, gray_t gray_image[height][width],
	      rgb_t rgb_image[height][width]);

/* display a RGB image */
extern int RGBDisplay(int height, int width, rgb_t image[height][width]);


#endif

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
