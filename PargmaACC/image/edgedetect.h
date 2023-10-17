/*==============================================================================
  
   Purpose:    algorithms for edge detection
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
	       53754 Sankt Augustin, Germany
               rudolf.berrendorf@fh-bonn-rhein-sieg.de
  
==============================================================================*/

#if !defined(ALG_EDGE_H_INCLUDED)
#define ALG_EDGE_H_INCLUDED

#include "commondefs.h"
#include "gray.h"

/*============================================================================*/
/* functions */

extern int
edgedetect_laplace (int height,        // I: image height
		    int width,         // I: image width
		    gray_t image[height][width],        // I: input image
		    gray_t new_image[height][width]    // O: output image;
		    );

extern int
edgedetect_sobel (int height,          // I: image height
		  int width,           // I: image width
		  gray_t gray_image[height][width],     // I: inout image
		  gray_t strength[height][width],       // O: strength values
		  gray_t direction[height][width]      // O: direction values
		  );


#endif

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
