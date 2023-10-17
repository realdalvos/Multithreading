/*==============================================================================

   Purpose:    algorithms for edge detection
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
         53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

/*============================================================================*/
/* The following algorithms are implemented:

   - edge detection with Laplace operator (edgedetect_laplace)
   - edge detection with Sobel operator (edgedetect_sobel)
     (3x3 and 5x5 operators)

   See for example: Th.Br�unl: Parallel Image Processing, Springer 2001
*/

/*============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <libFHBRS.h>

#include "edgedetect.h"
#include "gray.h"

#define PI 3.14159265358979323846

#define REFERENCE_CHECKSUM_LAPLACE 217015525UL
#define REFERENCE_CHECKSUM_SOBEL 302231239UL

/*============================================================================*/
// calculate checkum for an image

static unsigned long
checksum(int height,                 // I: image height
         int width,                  // I: image width
         gray_t image[height][width] // I: image
)
{
  unsigned long checksum = 0;
#pragma acc parallel loop collapse(2) reduction(+           \
                                                : checksum) \
    copyin(image [0:height] [0:width])
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; ++j)
      checksum += (unsigned long)image[i][j];

  return checksum;
}

/*============================================================================*/
/* edge detection with Laplace operator:
   img[i][j] = 4*img[i][j] - 4 neighbors cells

   ----------------
   |    | -1 |    |
   ----------------
   | -1 |  4 | -1 |
   ----------------
   |    | -1 |    |
   ----------------
 */

int edgedetect_laplace(int height,                      // I: image height
                       int width,                       // I: image width
                       gray_t image[height][width],     // I: input image
                       gray_t new_image[height][width]) // O: output image
{
  double t0, t1;

  // start timing
  t0 = gettime();
#pragma acc data parallel copyin(image [0:height] [0:width]) \
    copyout(new_image [0:height] [0:width])
  {
#pragma acc parallel
    {
// just for reproducable checksums...
#pragma acc loop collapse(2)
      for (int i = 0; i < height; i++)
      {
        for (int j = 0; j < width; j++)
        {
          new_image[i][j] = 0;
        }
      }
    } // acc parallel

#pragma acc parallel
    {
// just for reproducable checksums...
#pragma acc loop collapse(2)
      for (int i = 1; i < height - 1; i++)
      {
        for (int j = 1; j < width - 1; ++j)
        {
          // apply laplace operator
          unsigned int val = 4 * image[i][j] - image[i - 1][j] - image[i + 1][j] - image[i][j - 1] - image[i][j + 1];

          /* store calculated value (map to correct range) */
          new_image[i][j] = min(val, GRAY_MAX);
        }
      }
    } // acc parallel
  }   // acc data
  // end timing
  t1 = gettime();

  printf("time Laplace edge detection: %.6f s\n", t1 - t0);
  unsigned long cs = checksum(height, width, new_image);
  if (cs != REFERENCE_CHECKSUM_LAPLACE)
    printf("\t error checksum Laplace: expected %lu, seen %lu\n", REFERENCE_CHECKSUM_LAPLACE, cs);
  else
    printf("checksum Laplace OK        : %lu\n", cs);

  /*--------------------------------------------------------------------------*/

  /* everything ok */
  return 0;
}

/*============================================================================*/
/* Sobel 5x5 operator:

   x direction (direction=0):
   --------------------------------
   | -0.5 | -0.4 |  0 | 0.4 | 0.5 |
   --------------------------------
   | -0.8 | -1.0 |  0 | 1.0 | 0.8 |
   --------------------------------
   | -1.0 | -2.0 |  0 | 2.0 | 1.0 |
   --------------------------------
   | -0.8 | -1.0 |  0 | 1.0 | 0.8 |
   --------------------------------
   | -0.5 | -0.4 |  0 | 0.4 | 0.5 |
   --------------------------------


   y direction (direction=1):
   ------------------------------------
   |  0.5 |  0.8 |  1.0 |  0.8 |  0.5 |
   ------------------------------------
   |  0.4 |  1.0 |  2.0 |  1.0 |  0.4 |
   ------------------------------------
   |  0.0 |  0.0 |  0.0 |  0.0 |  0.0 |
   ------------------------------------
   | -0.4 | -1.0 | -2.0 | -1.0 | -0.4 |
   ------------------------------------
   | -0.5 | -0.8 | -1.0 | -0.8 | -0.5 |
   ------------------------------------
 */

void edgedetect_sobel5x5(int height,                  // I: image height
                         int width,                   // I: image width
                         gray_t image[height][width], // I: input image
                         int value[height][width],    // I: sobel output value
                         int direction                // I: horizontal or vertical
)
{
#pragma acc data copyin(image [0:height] [0:width]) copyout(value [0:height] [0:width])
  {
    if (direction == 0)
    {
/* x direction */
#pragma acc parallel loop collapse(2)
      for (int i = 2; i < height - 2; i++)
      {
        for (int j = 2; j < width - 2; j++)
        {
          // apply Sobel operator
          value[i][j] =
              (gray_t)(-0.5 * image[i - 2][j - 2] - 0.4 * image[i - 2][j - 1] + 0.4 * image[i - 2][j + 1] + 0.5 * image[i - 2][j + 2] - 0.8 * image[i - 1][j - 2] - 1.0 * image[i - 1][j - 1] + 1.0 * image[i - 1][j + 1] + 0.8 * image[i - 1][j + 2] - 1.0 * image[i][j - 2] - 2.0 * image[i][j - 1] + 2.0 * image[i][j + 1] + 1.0 * image[i][j + 2] - 0.8 * image[i + 1][j - 2] - 1.0 * image[i + 1][j - 1] + 1.0 * image[i + 1][j + 1] + 0.8 * image[i + 1][j + 2] - 0.5 * image[i + 2][j - 2] - 0.4 * image[i + 2][j - 1] + 0.4 * image[i + 2][j + 1] + 0.5 * image[i + 2][j + 2]);
        }
      }
    } // if
    else
    {
#pragma acc parallel
      {
/* y direction */
#pragma acc loop collapse(2)
        for (int i = 2; i < height - 2; i++)
        {
          for (int j = 2; j < width - 2; j++)
          {
            // apply Sobel operator
            value[i][j] =
                (gray_t)(+0.5 * image[i - 2][j - 2] + 0.8 * image[i - 2][j - 1] + 1.0 * image[i - 2][j] + 0.8 * image[i - 2][j + 1] + 0.5 * image[i - 2][j + 2] + 0.4 * image[i - 1][j - 2] + 1.0 * image[i - 1][j - 1] + 2.0 * image[i - 1][j] + 1.0 * image[i - 1][j + 1] + 0.4 * image[i - 1][j + 2] - 0.4 * image[i + 1][j - 2] - 1.0 * image[i + 1][j - 1] - 2.0 * image[i + 1][j] - 1.0 * image[i + 1][j + 1] - 0.4 * image[i + 1][j + 2] - 0.5 * image[i + 2][j - 2] - 0.8 * image[i + 2][j - 1] - 1.0 * image[i + 2][j + 1] - 0.8 * image[i + 2][j + 1] - 0.5 * image[i + 2][j + 2]);
          }
        }
      } // acc parallel
    }   // else
  }     // acc data
}
/*============================================================================*/
/* edge detection with Sobel operator */

int edgedetect_sobel(int height,                       // I: image height
                     int width,                        // I: image width
                     gray_t gray_image[height][width], // I: inout image
                     gray_t strength[height][width],   // O: strength values
                     gray_t direction[height][width]   // O: direction values
)
{
  double t0, t1;
  int tmp_image1[height][width];
  int tmp_image2[height][width];

#pragma acc data create(tmp_image1 [0:height] [0:width], tmp_image2 [0:height] [0:width], strength [0:height] [0:width], direction [0:height] [0:width])  \
    copyout(strength [0:height] [0:width], direction [0:height] [0:width])
  {
    /* use 5x5 Sobel */
#define edgedetect_sobel_filter(w, h, g, t, v) edgedetect_sobel5x5(w, h, g, t, v)

    // start timing
    t0 = gettime();
#pragma acc parallel
    {
// just for reproducable checksums...
#pragma acc loop collapse(2)
      {
        //#pragma acc loop collapse(2)
        for (int i = 0; i < height; i++)
        {
          for (int j = 0; j < width; j++)
          {
            tmp_image1[i][j] = tmp_image2[i][j] = 0;
          }
        }
      } // acc loop
    }   // acc prallel
        // acc data

    /* Sobel x direction */
    edgedetect_sobel_filter(height, width, gray_image, tmp_image1, 0);

    /* Sobel y direction */
    edgedetect_sobel_filter(height, width, gray_image, tmp_image2, 1);

/* calculate strength and direction */
#pragma acc parallel
    {
#pragma acc loop collapse(2)
      {
        for (int i = 0; i < height; i++)
        {
          for (int j = 0; j < width; j++)
          {
            /* strength (mapped to gray scale) */
            double dx = tmp_image1[i][j];
            double dy = tmp_image2[i][j];
            double tmp = (gray_t)sqrt(dx * dx + dy * dy);
            strength[i][j] = min(max(GRAY_MIN, tmp), GRAY_MAX);

            /* direction (mapped to gray scale) */
            tmp = (atan2(dy, dx) + PI) / (2.0 * PI) * (double)GRAY_MAX;
            direction[i][j] = (gray_t)tmp;
          }
        }
      } // acc loop
    }   // acc parallel
  }
  // end timing
  t1 = gettime();
  printf("time Sobel filter          : %.6f\n", t1 - t0);

  unsigned long cs = checksum(height, width, direction);
  // GPU floating point is not exact! Allow certain error margin.
  if (fabs((double)cs / (double)REFERENCE_CHECKSUM_SOBEL) - 1.0 > 0.001)
    printf("\t error checksum Sobel: expected %lu, seen %lu\n", REFERENCE_CHECKSUM_SOBEL, cs);
  else
    printf("checksum Sobel OK          : %lu\n", cs);

  /*--------------------------------------------------------------------------*/

  return 0;
}
// Vlad
/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
