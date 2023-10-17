/*==============================================================================
  
   Purpose:    functions on black and white images
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#if !defined(BLACKWHITE_H_INCLUDED)
#define BLACKWHITE_H_INCLUDED

/*============================================================================*/
/* types */

typedef unsigned char blackwhite_t;
#define BLACK 0
#define WHITE 1

#include "rgb.h"
#include "gray.h"

/*============================================================================*/
/* functions */

/* convert a RGB image to black and white image */
extern int RGBtoBw(int height, int width, rgb_t rgb_image[height][width],
		   int treshhold, blackwhite_t bw_image[height][width]);

/* convert a gray image to black and white image */
extern int graytoBw(int height, int width, gray_t gray_image[height][width],
		    int treshhold, blackwhite_t bw_image[height][width]);

/* invert a black and white image  */
extern int BWInvert(int height, int width, blackwhite_t bw_image1[height][width],
		    blackwhite_t bw_image2[height][width]);

/* display a black and white image */
extern int BWDisplay(int height, int width, blackwhite_t image[height][width]);

#endif

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
