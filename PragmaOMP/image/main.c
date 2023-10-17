/*==============================================================================
  
   Purpose:    main program
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
	       53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tiffio.h>            /* link with -ltiff */
#include <libFHBRS.h>          /* link with -lFHBRS -L/usr/X11R6/lib -lX11 */

#include "tiff.h"
#include "rgb.h"
#include "gray.h"
#include "edgedetect.h"

/*============================================================================*/
/* print usage information */

static void
usage(char *name)
{
  printf("usage: %s [-v] [-n] filename.tiff\n", name);
  printf("\t-v   be verbose\n");
  printf("\t-n   dont show result pictures\n");
  exit(EXIT_FAILURE);
}

/*============================================================================*/

int main(int argc, char **argv) {

  rgb_t *RGB_image;
  int width, height;
  int arg;
  int verbose = 0, show = 1;
  char *tiff_filename;


  if(argc < 2)
    usage(argv[0]);

  /* process command line arguments */
  for(arg=1; arg<argc; arg++) {
    if(!strcmp(argv[arg], "-v"))
      verbose = 1;
    else if(!strcmp(argv[arg], "-n"))
      show = 0;
    else if(argv[arg][0] == '-')
      usage(argv[0]);
    else
      break;
  }
  tiff_filename = argv[arg];


  /* read TIFF image file and convert it to RGB image */
  if(verbose)
    printf("reading TIFF file %s\n", tiff_filename);
  if(TiffReadToRGB(tiff_filename, &height, &width, &RGB_image) != 0) {
    printf("error while reading TIFF file\n");
    exit(EXIT_FAILURE);
  }
  if(verbose) {
    printf("Image size: %dx%d\n", height, width);
    /* display image */
    RGBDisplay(height, width, (rgb_t (*)[width])RGB_image);
  }

  // allocate working images
  gray_t *gray_image = malloc(height * width * sizeof(*gray_image));
  gray_t *edge_image = malloc(height * width * sizeof(*edge_image));
  gray_t *strength_image = malloc(height * width * sizeof(*strength_image));
  gray_t *direction_image = malloc(height * width * sizeof(*direction_image));
  if((gray_image == NULL) || (edge_image == NULL) || (strength_image == NULL) || (direction_image == NULL))
    {
      printf("No more memmory\n");
      exit(1);
    }

  /* convert RGB image to gray image */
  if(verbose)
    printf("Convert RGB to gray\n");
  RGBtoGray(height, width, (rgb_t (*)[width])RGB_image, (gray_t (*)[width])gray_image);
  if(verbose)
    /* display image */
    GrayDisplay(height, width, (gray_t (*)[width])gray_image);

  /* contrast stretching */
  GrayContrastStretching(height, width, (gray_t (*)[width])gray_image);
  if(verbose)
    /* display image */
    GrayDisplay(height, width, (gray_t (*)[width])gray_image);

  /* edge detection with Laplace */
  if(verbose)
    printf("edge detection with Laplace\n");
  edgedetect_laplace(height, width, (gray_t (*)[width])gray_image, (gray_t (*)[width])edge_image);
  if(verbose)
    /* display image */
    GrayDisplay(height, width, (gray_t (*)[width])edge_image);

  /* edge detection with Sobel */
  if(verbose)
    printf("edge detection with Sobel\n");
  edgedetect_sobel(height, width, (gray_t (*)[width])gray_image,
		   (gray_t (*)[width])strength_image, (gray_t (*)[width])direction_image);
  /* display image */
  if(show) {
    printf("edge strength image: \n");
    GrayDisplay(height, width, (gray_t (*)[width])strength_image);
    /* display image */
    printf("edge direction image: \n");
    GrayDisplay(height, width, (gray_t (*)[width])direction_image);
  }

  /* free data */
  free(RGB_image);

  return EXIT_SUCCESS;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
