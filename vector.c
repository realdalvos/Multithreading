/*==============================================================================

   Purpose          : vector addition
   Author           : Rudolf Berrendorf
                      Computer Science Department
                      Bonn-Rhein-Sieg University of Applied Sciences
                53754 Sankt Augustin, Germany
                      rudolf.berrendorf@h-brs.de

==============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include <libFHBRS.h>

#include <omp.h>

//==============================================================================
// typedefs

// type for vector values
typedef short value_t;
// type for vector dimension / indices
typedef long index_t;
// function type to combine two values
typedef value_t (*function_t)(const value_t x, const value_t y);

//==============================================================================
/** @brief our function to combine tow values
 * @param[in] x first value
 * @param[in] y secondd value
 * @return addition of the two values
 */
value_t add(const value_t x, const value_t y)
{
  return ((x + y) * (x - y)) % ((int)x + 1) + 27;
}

//==============================================================================
/** @brief allocate and initialize vectors
 * @param[in] n vector size
 * @param[out] a pointer to vector 1
 * @param[out] b pointer to vector 2
 * @param[out] c pointer to vector 3 (initialized with 0)
 */
void vectorInit(index_t n, value_t **a, value_t **b, value_t **c)
{

  // allocate memory
  *a = malloc(n * sizeof(**a));
  *b = malloc(n * sizeof(**b));
  *c = malloc(n * sizeof(**c));
  if ((*a == NULL) || (*b == NULL) || (*c == NULL))
  {
    printf("no more memory\n");
    exit(EXIT_FAILURE);
  }

  // initialize vectors
  #pragma omp parallel for
  for (index_t i = 0; i < n; i++)
  {
    (*a)[i] = (value_t)(2 * i);
    (*b)[i] = (value_t)(n - i);
    (*c)[i] = 0;
  }
}

//==============================================================================
/** @brief operate on two vectors sequentially
 * @param[in] n vector size
 * @param[in] a input vector 1
 * @param[in] b input vector 2
 * @param[out] c result vector
 * @param[in] f function to combine two values
 * @return sum of all vector elements in result vector
 */
value_t vectorOperation(index_t n, value_t a[n], value_t b[n], value_t c[n], function_t f)
{

  value_t sum = 0;

  for (index_t i = 0; i < n; i++)
  {
    sum += (c[i] = f(a[i], b[i]));
  }

  return sum;
}

//==============================================================================
/** @brief combine two vectors in parallel
 * @param[in] n vector size
 * @param[in] a input vector 1
 * @param[in] b input vector 2
 * @param[out] c result vector
 * @param[in] f function to combine two values
 * @param[in] p number of threads to use
 * @return sum of all vector elements in result vector
 */
value_t vectorOperationParallel(index_t n, value_t a[n], value_t b[n], value_t c[n], function_t f)
{

  // this version should be modified

  value_t sum = 0;
#pragma omp parallel for reduction(+:sum)
  for (index_t i = 0; i < n; i++)
  {
    sum += (c[i] = f(a[i], b[i]));
  }
  return sum;
}

//==============================================================================

int main(int argc, char **argv)
{
  // three vector variables
  value_t *a;
  value_t *b;
  value_t *c;

  // check for correct argument count
  if (argc != 3)
  {
    printf("usage: %s vector_size n_threads\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // get arguments
  // vector size
  index_t n = (index_t)atol(argv[1]);
  // number of threads
  int p = atoi(argv[2]);
  // check for plausible values
  if ((p < 1) || (p > 1000))
  {
    printf("illegal number of threads\n");
    exit(EXIT_FAILURE);
  }

  //-----------------------------------------------------------
  // sequential

  // allocate and initialize vectors a,b,c
  vectorInit(n, &a, &b, &c);

  // work on vectors sequentially
  double t0 = gettime();
  value_t c1sum = vectorOperation(n, a, b, c, add);
  t0 = gettime() - t0;

  // free memory
  free(a);
  free(b);
  free(c);

  //-----------------------------------------------------------
  // parallel

  // work on vectors parallel for all thread counts from 1 to p as powers of 2
  for (int thr = 1; thr <= p; thr *= 2)
  {
    // change default number of OpenMP threads
    omp_set_num_threads(thr);

    // allocate and initialize vectors a,b,c
    vectorInit(n, &a, &b, &c);

    // do operation
    double t1 = gettime();
    value_t c2sum = vectorOperationParallel(n, a, b, c, add);
    t1 = gettime() - t1;

    // free memory
    free(a);
    free(b);
    free(c);

    // check result
    if (c1sum != c2sum)
    {
      printf("!!! error: vector results are not identical !!!\nsum1=%ld, sum2=%ld\n", (long)c1sum, (long)c2sum);
      return EXIT_FAILURE;
    }
    else
    {
      // show timings
      printf("p=%2d, checksum=%2ld, sequential time: %9.6f, parallel time: %9.6f, speedup: %4.1f\n", thr, (long)c1sum, t0, t1, t0 / t1);
    }
  }

  return EXIT_SUCCESS;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
