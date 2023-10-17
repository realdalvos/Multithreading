/*==============================================================================

   Purpose:    sequential Mandelbrot
   Author:     Rudolf Berrendorf
               Computer Science Department
               Bonn-Rhein-Sieg University
         53754 Sankt Augustin, Germany
               rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <libFHBRS.h>

#define X_RESOLUTION 1000 /* number of pixels in x-direction */
#define Y_RESOLUTION 800  /* number of pixels in y-direction */

/*==============================================================================*/

// set to 1 to display graphics / 0 to suppress graphical output
static int display = 1;
// calculate checksum
static unsigned long checksum = 0;
#define REFERENCE_CHECKSUM 3775983838UL

/*==============================================================================*/
/* get command line arguments
   a.out xmin ymin xmax ymax maxiter
   example a.out -2 -2 2 2 1000
 */

static void get_arguments(int argc,     // I: argument count
                          char **argv,  // I: argument vector
                          double *xmin, // O: minimum x coordinate
                          double *ymin, // O: minimum y ccordinate
                          double *xmax, // O: maximum x coordinate
                          double *ymax, // O: maximum y coordinate
                          int *maxiter, // O: maximum number of iterations
                          int *display  // O: show graphic
)
{
  if (argc != 7)
  {
    fprintf(stderr, "%s: wrong command line arguments\n", argv[0]);
    fprintf(stderr, "usage: %s xmin ymin xmax ymax maxiter display\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  *xmin = atof(argv[1]);
  *ymin = atof(argv[2]);
  *xmax = atof(argv[3]);
  *ymax = atof(argv[4]);
  *maxiter = atoi(argv[5]);
  *display = atoi(argv[6]);
}

/*==============================================================================*/
/* draw point */

static void
drawPoint(int window, int i, int j, int anziter, int maxiter)
{
  // update checksum
  checksum += anziter;

  if (display)
  {
    if (anziter == maxiter)
      graphic_setColor(window, GRAPHIC_MAX_COLOR - 1);
    else
      graphic_setColor(window, anziter % GRAPHIC_MAX_COLOR);
    graphic_drawPoint(window, i, j);
  }
}

/*==============================================================================*/
/* main program */

int main(int argc, char **argv)
{
  int i, j, k, maxiter;
  double absvalue, temp;
  struct
  {
    double real, imag;
  } z, c;
  double xmin, ymin, xmax, ymax;
  double dx, dy;
  double t_start, t_end;
  char ch;
  int window = 0;

  /* initialisation */
  get_arguments(argc, argv, &xmin, &ymin, &xmax, &ymax, &maxiter, &display);

  if (display)
    window = graphic_start(X_RESOLUTION, Y_RESOLUTION, argv[0]);
  dx = (xmax - xmin) / X_RESOLUTION;
  dy = (ymax - ymin) / Y_RESOLUTION;
  printf("display of (%.5f,%.5f)-(%.5f,%.5f) in steps of (%.5f,%.5f)\n",
         xmin, ymin, xmax, ymax, dx, dy);

  /* get start time */
  t_start = gettime();

/*--------------------------------------------------------------------------*/

/* calculate values for every point in complex plane */
#pragma omp parallel for private(j, k, z, c, temp, absvalue)
  for (i = 0; i < X_RESOLUTION; i++)
  {
    for (j = 0; j < Y_RESOLUTION; j++)
    {
      /* map point to window */
      c.real = xmin + i * dx;
      c.imag = ymin + j * dy;
      z.real = z.imag = 0.0;
      k = 0;

      // do iterations for point i,j
      do
      {
        temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp;
        absvalue = z.real * z.real + z.imag * z.imag;
        k++;
      } while (absvalue < 4.0 && k < maxiter);

// draw point i,j
#pragma omp critical
      drawPoint(window, i, j, k, maxiter);
    }
    if (display)
    // flush graphic
#pragma omp critical
      graphic_flush(window);
  }

  /*--------------------------------------------------------------------------*/
  /* get end time */
  t_end = gettime();
  printf("calculation took %.2f s\n", t_end - t_start);
  if (checksum != REFERENCE_CHECKSUM)
    printf("\t!!!!! error: checksum wrong.\n\texpected:%lu, seen: %lu\n", REFERENCE_CHECKSUM, checksum);
  else
    printf("checksum OK: %lu\n", checksum);

  if (display)
  {
    printf("continue? (any key) ");
    scanf("%c", &ch);
    graphic_end(window);
  }

  return EXIT_SUCCESS;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
