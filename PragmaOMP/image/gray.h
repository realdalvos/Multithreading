/*==============================================================================
  
   Purpose:    functions on gray data
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University of Applied Sciences
               53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#if !defined(GRAY_H_INCLUDED)
#define GRAY_H_INCLUDED

#include "commondefs.h"

#define GRAY_MIN ((gray_t)0)         /* minimum gray value */
#define GRAY_MAX ((gray_t)255)       /* maximum gray value */

/*============================================================================*/
/* types */

/* type to store gray values */
typedef unsigned char gray_t;

/*============================================================================*/
/* functions */

#include "blackwhite.h"
#include "rgb.h"

/* convert a RGB image to a gray image */
extern int RGBtoGray(int height, int width, rgb_t rgb_image[height][width],
		     gray_t gray_image[height][width]);

/* convert a RGB image to a gray image */
extern int BWtoGray(int height, int width, blackwhite_t bw_image[height][width],
		    gray_t gray_image[height][width]);

/* invert gray image */
int GrayInvert(int height, int width, gray_t image[height][width]);

/* contrast stretching: range of gray values is extended */
int GrayContrastStretching(int height, int width, gray_t image[height][width]);

/* mean of 3x3 neighborhood */
int GrayMean_3x3(int height, int width, gray_t image[height][width],
		 gray_t new_image[height][width]);

/* median of 3x3 neighborhood */
int GrayMedian_3x3(int height, int width, gray_t image[height][width],
		   gray_t new_image[height][width]);

/* median of 5x5 neighborhood */
int GrayMedian_5x5(int height, int width, gray_t image[height][width],
		   gray_t new_image[height][width]);

/* display a gray image */
extern int GrayDisplay(int height, int width, gray_t image[height][width]);

#endif

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
