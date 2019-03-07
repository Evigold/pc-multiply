/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 * 
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the 
 *  second matrix row count.  
 *
 *  A matrix is consumed from the bounded buffer.  Then matrices are consumed
 *  from the bounded buffer, one at a time, until an eligible matrix for multiplication
 *  is found.
 *
 *  Totals are tracked using the ProdConsStats Struct for:
 *  - the total number of matrices multiplied (multtotal from consumer threads)
 *  - the total number of matrices produced (matrixtotal from producer threads)
 *  - the total number of matrices consumed (matrixtotal from consumer threads)
 *  - the sum of all elements of all matrices produced and consumed (sumtotal from producer and consumer threads)
 *
 *  Correct programs will produce and consume the same number of matrices, and
 *  report the same sum for all matrix elements produced and consumed. 
 *
 *  For matrix multiplication only ~25% may be e
 *  and consume matrices.  Each thread produces a total sum of the value of
 *  randomly generated elements.  Producer sum and consumer sum must match.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Winter 2019
 *  Eviatar Goldschmidt
 *  Scott Robertson
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main(int argc, char *argv[])
{
  // Process command line arguments
  int numw = NUMWORK;
  if (argc == 1)
  {
    BOUNDED_BUFFER_SIZE = MAX;
    NUMBER_OF_MATRICES = LOOPS;
    MATRIX_MODE = DEFAULT_MATRIX_MODE;
    printf("USING DEFAULTS: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw, BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }
  else
  {
    if (argc == 2)
    {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = MAX;
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 3)
    {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = LOOPS;
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 4)
    {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = DEFAULT_MATRIX_MODE;
    }
    if (argc == 5)
    {
      numw = atoi(argv[1]);
      BOUNDED_BUFFER_SIZE = atoi(argv[2]);
      NUMBER_OF_MATRICES = atoi(argv[3]);
      MATRIX_MODE = atoi(argv[4]);
    }
    printf("USING: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n", numw, BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  } 


   time_t t;
  // Seed the random number generator with the system time
  srand((unsigned)time(&t));



  
  
  printf("Producing %d matrices in mode %d.\n", NUMBER_OF_MATRICES, MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n", numw);
  printf("\n");

  // Declares an array of threads for both the producer and consumer threads.
  pthread_t prod[NUMWORK];
  pthread_t con[NUMWORK];
  
  // Counters struct that will be passed to the threads as void *args.
  // Keeps track of the count of matrixies produced, consumed and successfully multplied.
  // Keeps track of the sum of teh elements of all the matrixies generated and consumed.
  counters_t stats;
  counter_t prodCount;
  counter_t conCount;
  counter_t prodSum;
  counter_t conSum;
  counter_t multCount;
  
  init_cnt(&prodCount);
  init_cnt(&conCount);
  init_cnt(&prodSum);
  init_cnt(&conSum);
  init_cnt(&multCount);

  stats.prodCount = &prodCount;
  stats.consCount = &conCount;
  stats.prodSum = &prodSum;
  stats.consSum = &conSum;
  stats.multCount = &multCount;

  // Creates all the threads.
  for(int i = 0; i < NUMWORK; i++) {
    pthread_create(&prod[i], NULL, prod_worker, &stats);
    pthread_create(&con[i], NULL, cons_worker, &stats);
  }
  // Disposes of all the threads.
  for(int i = 0; i < NUMWORK; i++) {
    pthread_join(prod[i], NULL);
    pthread_join(con[i], NULL);
  }
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", get_cnt(stats.prodSum), get_cnt(stats.consSum));
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", get_cnt(stats.prodCount), get_cnt(stats.consCount), get_cnt(stats.multCount));


  return 0;
}