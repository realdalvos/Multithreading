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
#include <omp.h>
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
//#pragma omp parallel
  {
//#pragma omp for collapse(2) reduction(+:checksum)
    for (int i = 0; i < height; i++)
      for (int j = 0; j < width; ++j)
        checksum += (unsigned long)image[i][j];
  }
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
  int i, j;
  unsigned int val;
  double t0, t1;

  // start timing
  t0 = gettime();

  // just for reproducable checksums...
//#pragma omp parallel
  {
//#pragma omp for collapse(2) firstprivate(i, j, new_image)
    for (i = 0; i < height; i++)
    {
      for (j = 0; j < width; j++)
      {
        new_image[i][j] = 0;
      }
    }
  }
//#pragma omp parallel
  {
//#pragma omp for collapse(2) firstprivate(i, j, val)
    for (i = 1; i < height - 1; i++)
    {
      for (j = 1; j < width - 1; ++j)
      {
        // apply laplace operator
        val = 4 * image[i][j] - image[i - 1][j] - image[i + 1][j] - image[i][j - 1] - image[i][j + 1];

        /* store calculated value (map to correct range) */
        new_image[i][j] = min(val, GRAY_MAX);
      }
    }
  }
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

static void edgedetect_sobel5x5(int height,                  // I: image height
                                int width,                   // I: image width
                                gray_t image[height][width], // I: input image
                                int value[height][width],    // I: sobel output value
                                int direction                // I: horizontal or vertical
)
{
  int i, j;

  if (direction == 0)
  {

    /* x direction */
//#pragma omp parallel
    {
//#pragma omp for collapse(2) firstprivate(i, j)
      for (i = 2; i < height - 2; i++)
      {
        for (j = 2; j < width - 2; j++)
        {
          // apply Sobel operator
          value[i][j] =
              (gray_t)(-0.5 * image[i - 2][j - 2] - 0.4 * image[i - 2][j - 1] + 0.4 * image[i - 2][j + 1] + 0.5 * image[i - 2][j + 2] - 0.8 * image[i - 1][j - 2] - 1.0 * image[i - 1][j - 1] + 1.0 * image[i - 1][j + 1] + 0.8 * image[i - 1][j + 2] - 1.0 * image[i][j - 2] - 2.0 * image[i][j - 1] + 2.0 * image[i][j + 1] + 1.0 * image[i][j + 2] - 0.8 * image[i + 1][j - 2] - 1.0 * image[i + 1][j - 1] + 1.0 * image[i + 1][j + 1] + 0.8 * image[i + 1][j + 2] - 0.5 * image[i + 2][j - 2] - 0.4 * image[i + 2][j - 1] + 0.4 * image[i + 2][j + 1] + 0.5 * image[i + 2][j + 2]);
        }
      }
    }
  }
  else
  {

    /* y direction */
//#pragma omp parallel
    {
//#pragma omp for collapse(2) firstprivate(i, j, value)
      for (i = 2; i < height - 2; i++)
      {
        for (j = 2; j < width - 2; j++)
        {
          // apply Sobel operator
          value[i][j] =
              (gray_t)(+0.5 * image[i - 2][j - 2] + 0.8 * image[i - 2][j - 1] + 1.0 * image[i - 2][j] + 0.8 * image[i - 2][j + 1] + 0.5 * image[i - 2][j + 2] + 0.4 * image[i - 1][j - 2] + 1.0 * image[i - 1][j - 1] + 2.0 * image[i - 1][j] + 1.0 * image[i - 1][j + 1] + 0.4 * image[i - 1][j + 2] - 0.4 * image[i + 1][j - 2] - 1.0 * image[i + 1][j - 1] - 2.0 * image[i + 1][j] - 1.0 * image[i + 1][j + 1] - 0.4 * image[i + 1][j + 2] - 0.5 * image[i + 2][j - 2] - 0.8 * image[i + 2][j - 1] - 1.0 * image[i + 2][j + 1] - 0.8 * image[i + 2][j + 1] - 0.5 * image[i + 2][j + 2]);
        }
      }
    }
  }
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
  int i, j;
  double t0, t1;
  double dx, dy, tmp;
  int tmp_image1[height][width];
  int tmp_image2[height][width];

  /* use 5x5 Sobel */
#define edgedetect_sobel_filter(w, h, g, t, v) edgedetect_sobel5x5(w, h, g, t, v)

  // start timing
  t0 = gettime();

// just for reproducable checksums...
//#pragma omp parallel
  {
//#pragma omp for collapse(2) firstprivate(i, j, tmp_image1, tmp_image2)
    for (i = 0; i < height; i++)
    {
      for (j = 0; j < width; j++)
      {
        tmp_image1[i][j] = tmp_image2[i][j] = 0;
      }
    }
  }

  /* Sobel x direction */
  edgedetect_sobel_filter(height, width, gray_image, tmp_image1, 0);

  /* Sobel y direction */
  edgedetect_sobel_filter(height, width, gray_image, tmp_image2, 1);

/* calculate strength and direction */
#pragma omp parallel for
  {
    for (i = 0; i < height; i++)
    {
      for (j = 0; j < width; j++)
      {
        /* strength (mapped to gray scale) */
        dx = tmp_image1[i][j];
        dy = tmp_image2[i][j];
        tmp = (gray_t)sqrt(dx * dx + dy * dy);
        strength[i][j] = min(max(GRAY_MIN, tmp), GRAY_MAX);

        /* direction (mapped to gray scale) */
        tmp = (atan2(dy, dx) + PI) / (2.0 * PI) * (double)GRAY_MAX;
        direction[i][j] = (gray_t)tmp;
      }
    }
  }
  // end timing
  t1 = gettime();
  printf("time Sobel filter          : %.6f\n", t1 - t0);

  unsigned long cs = checksum(height, width, direction);
  if (cs != REFERENCE_CHECKSUM_SOBEL)
    printf("\t error checksum Sobel: expected %lu, seen %lu\n", REFERENCE_CHECKSUM_SOBEL, cs);
  else
    printf("checksum Sobel OK          : %lu\n", cs);

  /*--------------------------------------------------------------------------*/

  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
