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
 *  Fall 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "counter.h"
#include "matrix.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main(int argc, char *argv[])
{
  // Process command line arguments
  int i, numw = NUMWORK;
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


  //Pointer to pointer matrix
  bigmatrix = (Matrix **)malloc(sizeof(Matrix *) * BOUNDED_BUFFER_SIZE);


  time_t t;
  // Seed the random number generator with the system time
  srand((unsigned)time(&t));



  
  
  printf("Producing %d matrices in mode %d.\n", NUMBER_OF_MATRICES, MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n", numw);
  printf("\n");

  pthread_t prod[numw*2];
  // pthread_t con[numw];
  
  // consume ProdConsStats from producer and consumer threads
  // add up total matrix stats in prs, cos, prodtot, constot, consmul
  counters_t counters;
  // init_cnts(&counters);
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
  counters.cons = &conCount;
  counters.prod = &prodCount;
  counters.conSum = &conSum;
  counters.prodSum = &prodSum;
  counters.mult = &multCount;
 
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", get_cnt(counters.prodSum), get_cnt(counters.conSum));
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", get_cnt(counters.prod), get_cnt(counters.cons), get_cnt(counters.mult));

  for (i = 0; i < numw * 2; i++) {
    if(i % 2 == 0)  pthread_create(&prod[i], NULL, prod_worker,  (void *)&counters);
    else pthread_create(&prod[i], NULL, cons_worker,  (void *)&counters);
  }
  
  for (i = 0; i < numw * 2; i++) {
    pthread_join(prod[i], NULL);
  }
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", get_cnt(counters.prodSum), get_cnt(counters.conSum));
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", get_cnt(counters.prod), get_cnt(counters.cons), get_cnt(counters.mult));


  return 0;
}

