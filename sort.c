/*==============================================================================
  
   Purpose          : sort with MPI
   Author           : Rudolf Berrendorf
                      Computer Science Department
                      Bonn-Rhein-Sieg University of Applied Sciences
	              53754 Sankt Augustin, Germany
                      rudolf.berrendorf@h-brs.de
  
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <mpi.h>
#include <libFHBRS.h>

// type for indices and dimensions
typedef int idx_t;

// uncomment to get debug output (run only with small data sizes!)
//#define DEBUG 1


//==============================================================================
// runtime error check macros

// check for MPI error
#define checkMpiError(err) {if((err) != MPI_SUCCESS) {printf("MPI error in file %s, line %d\n", __FILE__, __LINE__);}}
// check for true and print error message if not
#define check(b) {if(!(b)) {printf("runtime error in file %s, line %d\n", __FILE__, __LINE__);}}


//==============================================================================
/** determine local block size such that an index space is distributed evenly
 * over all processes (and filled up with one additional element if necessary)
 * @param n global size
 * @param p number of processes
 * @return local block size
 */
static idx_t localBlockSize(idx_t n, idx_t p) {
  // determine local block size
  idx_t blockSize = n / p;
  if(n % p != 0) {
    blockSize++;
  }

  return blockSize;
}


//==============================================================================
/** create a distributed array of size n and fill with random values in [0,n)
 * @param n (accumulated) global size of array
 * @param a pointer to an array variable that gets the address of the newly created array
 */

static void createArray(idx_t n, int **a) {
  // get MPI rank and size
  int rank;
  int size;
  checkMpiError(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
  checkMpiError(MPI_Comm_size(MPI_COMM_WORLD, &size));

  // determine local block size
  idx_t blockSize = localBlockSize(n, (idx_t)size);
#if defined(DEBUG)
  printf("[%3d] local block size=%u\n", rank, (unsigned int)blockSize);
#endif

  // create my local part of the distributed array
  *a = malloc(blockSize * sizeof(**a));
  check(*a != NULL);

  // rank 0 produces all values and distributes them to all other processes.
  // This is necessary to get the same random values, independent of the number
  // of participating processes.
  if(rank == 0) {
    // buffer to send generated random values to other processes
    int *buf = malloc(blockSize * sizeof(*buf));
    check(buf != NULL);

#if defined(DEBUG)
    printf("[%3d] original data: ", rank);
#endif

    // rank 0 produces random values and sends it to each process
    for(int proc=0; proc<size; proc++) {
      // generate random values
      for(idx_t i=0; i<blockSize; i++) {
        buf[i] = rand() % n;
#if defined(DEBUG)
        printf("%d ", buf[i]);
#endif
      }

      // copy or send
      if(proc == 0) {
        // copy to my own array
        for(idx_t i=0; i<blockSize; i++) {
          (*a)[i] = buf[i];
        }
      } else {
        // send to other process
        checkMpiError(MPI_Send(buf, blockSize, MPI_INT, proc, 4711, MPI_COMM_WORLD));
      }
    }

#if defined(DEBUG)
    printf("\n");
#endif

    // free buffer
    free(buf);

  } else {
    // all ranks other than 0
    // get random values from process 0
    MPI_Status status;
    checkMpiError(MPI_Recv(*a, blockSize, MPI_INT, 0, 4711, MPI_COMM_WORLD, &status));
#if defined(DEBUG)
    printf("[%3d] local array part received\n", rank);
#endif
  }

#if defined(DEBUG)
  checkMpiError(MPI_Barrier(MPI_COMM_WORLD));
  printf("[%3d] create array finished\n", rank);
  checkMpiError(MPI_Barrier(MPI_COMM_WORLD));
#endif
}

//==============================================================================
/** check whether a distributed array is sorted
 * This is done with checking
 * - that the local part is sorted
 * - that the last value of process i and the first value of process i+1 are sorted
 * @param n (accumulated) size of array
 * @param, a local partz of distributed array
 */
static void checkSorted(idx_t n, int *a) {
  int rank;
  int size;
  checkMpiError(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
  checkMpiError(MPI_Comm_size(MPI_COMM_WORLD, &size));

  if(rank == 0) {
#if defined(DEBUG)
    printf("{%3d} sorted data: ", rank);
#endif
    for(idx_t i=1; i<n; i++) {
      if(a[i-1] > a[i]) {
        printf("[%3d] array is not sorted at position %lu / %lu: %d > %d\n", rank, (unsigned long)i-1, (unsigned long)i, a[i-1], a[i]);
        return;
      }
#if defined(DEBUG)
      printf("%d ", a[i]);
#endif
    }
#if defined(DEBUG)
      printf("\n");
#endif
    printf("[%3d] array is sorted\n", rank);
  }
}


//==============================================================================
/** merge two sorted arrays into a third array
 * @param n size of the two input arrays
 * @param src1 pointer to array 1 of size n
 * @param src2 pointer to array 2 of size n
 * @param dest pointer to an array of size 2*n that gets filled with the sorted values
 */
static void merge(idx_t n, int *src1, int *src2, int *dest) {
  // read index into src1
  idx_t i = 0;
  // read index into src2
  idx_t j = 0;
  // write index into dest
  idx_t k = 0;

  // touch all values
  for(idx_t c=0; c<2*n; c++) {
    if(i<n && j<n) {
      // in both src1 and src2 are still values to be copied
      if(src1[i] < src2[j]) {
        dest[k++] = src1[i++];
      } else {
        dest[k++] = src2[j++];
      }
    } else if (i<n) {
      // all src2 values already copied
      dest[k++] = src1[i++];
    } else {
      // all src1 values already copied
      dest[k++] = src2[j++];
    }
  }
}

//==============================================================================
/** compare two integer and return <0,=0,>0 if x is small,equal,larger than y
 * @param x pointer to integer value 1
 * @param y pointer to integer value 2
 * @return  <0,=0,>0 if x is small,equal,larger than y
 */

static int compar(const void *x, const void *y) {
  return (*(int *)x) - (*(int *)y);
}

//==============================================================================
/** sort a distributed array
 * @param n size of array
 * @param a pointer to an array to be sorted. The sort function may return a
 * different pointer value in a, if it replaces the array by another (larger) one.
 */

static void sort(idx_t n, int**a) {
  // get MPI rank and size
  int rank;
  int size;
  checkMpiError(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
  checkMpiError(MPI_Comm_size(MPI_COMM_WORLD, &size));

  idx_t blockSize = localBlockSize(n, (idx_t)size);
  qsort(*a , blockSize , sizeof(**a), compar);
  
  // log2(size) rounds
  for (int stride = size/2; stride>0; stride/=2, blockSize*=2) {
    if(rank < stride) {
       // buffer to receive from other process
      int *buf = malloc(blockSize * sizeof(*buf));
      check(buf != NULL);
      // receive from partner and merge
      MPI_Status status;
      checkMpiError(MPI_Recv(buf, blockSize, MPI_INT, rank + stride, 4711, MPI_COMM_WORLD, &status));
      // buffer to receive from other process
      int *newA = malloc(2 * blockSize * sizeof(*newA));    //[0,1,2,3,4,5,......,15]
      check(newA != NULL);
      merge(blockSize, *a, buf, newA);
      free(buf);
      free(*a);
      *a = newA;
    }
    else if ((rank >= stride) && (rank < 2*stride)) {
      // send to partner
      checkMpiError(MPI_Send(*a, blockSize, MPI_INT, rank - stride, 4711, MPI_COMM_WORLD));
    }
    else {
      // nothing to do
    }
  }
  /*
    Here comes your code.

    Before you return, you must assign *a the pointer of the resulting array on process 0. I.e.
    if(rank == 0) {
        *a = ...
    }
   */

}

//==============================================================================
/** main program. Reads in array size n, generates a distributed array of size
    n, calls the sort function and checks whether the array (on process 0) is
    sorted.
 */

int main(int argc, char **argv)
{
  // array to be sorted
  int *a;

  if(argc != 2) {
    printf("usage: %s log2_arraySize\n", argv[0]);
    exit(1);
  }

  // get program arguments (log_2(n) is passed)
  idx_t n = ((idx_t)1) << atol(argv[1]);
  // we assume here that n and n / p fits into idx_t

  // initialize MPI
  checkMpiError(MPI_Init(&argc, &argv));
  int rank;
  int size;
  checkMpiError(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
  checkMpiError(MPI_Comm_size(MPI_COMM_WORLD, &size));

  // create a distributed array with random values
  createArray(n, &a);

  // sort. Afterwards process 0 has the resulting sorted array.
  // Keep in mind, that in sort the pointer to the array a changes as new
  // memory must be allocated to merge two array into a new larger array that replaces
  // the old array a
  double t = gettime();
  sort(n, &a);
  t = gettime() - t;
  if(rank == 0) {
    printf("[%3d] n=%12lu, p=%4d, sort=%12.6f s\n", rank, (unsigned long)n, size, t);
  }

  // check result
  checkSorted(n, a);

  // free memory
  free(a);

  checkMpiError(MPI_Finalize());
  return 0;
}

/*============================================================================*
 *                             that's all folks                               *
 *============================================================================*/
