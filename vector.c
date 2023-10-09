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

#include <pthread.h>
#include <libFHBRS.h>

//==============================================================================
// typedefs

// type for vector values
typedef short value_t;
// type for vector dimension / indices
typedef long index_t;
// function type to combine two values
typedef value_t (*function_t)(const value_t x, const value_t y);

// parameters to be passed to worker threads
typedef struct{
    value_t *a;
    value_t *b;
    value_t *c;
    long *sum;
    pthread_mutex_t *mutexPtr;
    function_t f;
  } ParamType;
  
typedef struct{
    index_t startPosition;
    index_t endPosition;
    ParamType params;
  } ThParamType;

//==============================================================================
/** @brief our function to combine two values
 * @param[in] x first value
 * @param[in] y secondd value
 * @return addition of the two values
 */
value_t add(const value_t x, const value_t y) {
  return ((x+y)*(x-y)) % ((int)x+1) + 27;
}


//==============================================================================
/** @brief initialize vectors
 * @param[in] n vector size
 * @param[out] a vector 1
 * @param[out] b vector 2
 * @param[out] c vector 3 (initialized with 0)
 */
void vectorInit(index_t n, value_t a[n], value_t b[n], value_t c[n]) {

  for(index_t i=0; i<n; i++) {
    a[i] = (value_t)(2*i);
    b[i] = (value_t)(n-i);
    c[i] = 0;
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
value_t vectorOperation(index_t n, value_t a[n], value_t b[n], value_t c[n], function_t f) {

  value_t sum = 0;

  for(index_t i=0; i<n; i++) {
    sum += (c[i] = f(a[i], b[i]));
  }

  return sum;
}

//==============================================================================
/** @brief function to be used in separate thread
 * @param[arg] ThParamType pointer
*/
void *work(void *arg) {
  
  // Kopieren des Thread-spezifischen Argumentes
  ThParamType* thParamPtr = (ThParamType*)arg;

  value_t *a = thParamPtr->params.a;
  value_t *b = thParamPtr->params.b;
  value_t *c = thParamPtr->params.c;
  function_t f = thParamPtr->params.f;
  long *sumPtr = thParamPtr->params.sum;
  /*
  pthread_mutex_lock(thParamPtr->params.mutexPtr);
  printf("Starting thread with a=%2ld, b=%2ld, c=%2ld, start=%2ld, end=%2ld\n", a, b, c, thParamPtr->startPosition, thParamPtr->endPosition);
  pthread_mutex_lock(thParamPtr->params.mutexPtr);
  */
  /*
  value_t localSum = vectorOperation( thParamPtr->endPosition - thParamPtr->startPosition,
                                      a+thParamPtr->startPosition,
                                      b+thParamPtr->startPosition,
                                      c+thParamPtr->startPosition,
                                      thParamPtr->params.f );
  */
  
  long localSum = 0;
  for(index_t i=thParamPtr->startPosition; i<thParamPtr->endPosition; i++){
    localSum += ( c[i] = f(a[i],b[i]) );
  }
  
  pthread_mutex_lock(thParamPtr->params.mutexPtr);
  ( *sumPtr ) += localSum;
  //printf("Terminating thread with start=%2ld, end=%2ld, localSum=%2ld, totalSum=%2ld\n", thParamPtr->startPosition, thParamPtr->endPosition, localsum, *(thParamPtr->params.sum) );
  pthread_mutex_unlock(thParamPtr->params.mutexPtr);
  
  return NULL;
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
value_t vectorOperationParallel(index_t n, value_t a[n], value_t b[n], value_t c[n], function_t f, int p) {

  long sum = 0;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&mutex, NULL);
  // this version should be modified

  pthread_t thr[p];
  int *status[p];

  ThParamType* arg = malloc( p * sizeof(ThParamType) );

  if(arg == NULL) {
    printf("no more memory\n");
    exit(EXIT_FAILURE);
  }

  ParamType params;
  params.a = a;
  params.b = b;
  params.c = c;
  params.sum = &sum;
  params.mutexPtr = &mutex;
  params.f = f;

  if( n%p!=0 ){
    if(p>=n){
      for(int i = 0 ; i < p; i++) {
        arg[i].startPosition = i;
        arg[i].endPosition = i+1;
        arg[i].params = params;
      }
    }else{
      for(int i = 0 ; i < p; i++) {
          if(i<(n%p)){
            arg[i].startPosition = (n/p+1)*i;
            arg[i].endPosition = (n/p+1)*(i+1);
            arg[i].params = params;
          }else{
            arg[i].startPosition = (n/p)*i+n%p;
            arg[i].endPosition = (i+1)*(n/p)+n%p;
            arg[i].params = params;
          }
       }
    }
  }else{
      for(int i = 0 ; i < p; i++) {
        arg[i].startPosition = i*(n/p);
        arg[i].endPosition = (i+1)*(n/p);
        arg[i].params = params;
      }
  }
  
  for(int i = 0; i < p; i++) {
    pthread_create(&thr[i], NULL, work, (void *)&arg[i]);
  }
  
  for(int i = 0; i < p; i++) {
    pthread_join(thr[i], (void **)&status[i]);
  }

  free ( arg );
  pthread_mutex_destroy(&mutex);

  return sum;
}


//==============================================================================

int main(int argc, char **argv)
{
  // check for correct argument count
  if (argc != 3)
    {
      printf ("usage: %s vector_size n_threads\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  // get arguments
  // vector size
  index_t n = (index_t)atol (argv[1]);
  // number of threads
  int p = atoi (argv[2]);
  // check for plausible values
  if((p < 1) || (p > 1000)) {
      printf("illegal number of threads\n");
      exit (EXIT_FAILURE);
  }

  // allocate memory
  value_t *a = malloc(n * sizeof(*a));
  value_t *b = malloc(n * sizeof(*b));
  value_t *c = malloc(n * sizeof(*c));
  if((a == NULL) || (b == NULL) || (c == NULL)) {
    printf("no more memory\n");
    exit(EXIT_FAILURE);
  }

  // initialize vectors a,b,c
  vectorInit(n, a, b, c);

  // work on vectors sequentially
  double t0 = gettime();
  value_t c1sum = vectorOperation(n, a, b, c, add);
  t0 = gettime() - t0;


  // work on vectors parallel for all thread counts from 1 to p
  for(int thr=1; thr<= p; thr*=2) {

    // re-initialize vectors a,b,c
    vectorInit(n, a, b, c);

    // do operation
    double t1 = gettime();
    value_t c2sum = vectorOperationParallel(n, a, b, c, add, thr);
    t1 = gettime() - t1;
    
    // check result
    if(c1sum != c2sum) {
      printf("!!! error: vector results are not identical !!!\nsum1=%ld, sum2=%ld\n", (long)c1sum, (long)c2sum);
      return EXIT_FAILURE;
    } else {
      // show timings
      printf("p=%2d, checksum=%2ld, sequential time: %9.6f, parallel time: %9.6f, speedup: %4.1f\n", thr, (long)c2sum, t0, t1, t0/t1);
    }
  }

  return EXIT_SUCCESS;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
