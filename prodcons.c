/*
 *  prodcons module
 *  Producer Consumer module
 * 
 *  Implements routines for the producer consumer module based on 
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


int count = 0;
int loops = LOOPS;
Matrix * buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int producing = 1;
int consuming = 0;


// Define Locks and Condition variables here
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; //BB is empty.
pthread_cond_t full = PTHREAD_COND_INITIALIZER;  //BB is full.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buflock = PTHREAD_MUTEX_INITIALIZER;  //Move a lock to get/put methods?

// Producer consumer data structures
ProdConsStats pcStats;

// Bounded buffer bigmatrix defined in prodcons.h
// Matrix ** bigmatrix;
// bigmatrix is inmitialized in main... 

// Bounded buffer put() get()
int put(Matrix * mat)
{
  
  if (mat != NULL) {
    buffer[fill_ptr] = mat;
    use_ptr = fill_ptr; //Lock issue here, need to update the use pointer to use a made matrix.
    printf("%d\n", fill_ptr);
    // GenMatrix(buffer[fill_ptr]);    
    fill_ptr = (fill_ptr + 1) % MAX;  
    pthread_cond_signal(&full); //Room in buffer
    return 1;
  }
  return 0;
}

Matrix * get() 
{
  Matrix * tmp = buffer[use_ptr];
  fill_ptr = use_ptr;
  use_ptr = (use_ptr - 1) % MAX;
  if (use_ptr < 2) {
    pthread_cond_signal(&empty); //Different condition? signal that there is nothing in buffer now.
  }
  
  DisplayMatrix(buffer[use_ptr], stdout);
  return tmp;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *counters)
{
  counters_t *c = (counters_t*)counters;
  int i;
  for (i = 0; i < loops; i++) {
    
    pthread_mutex_lock(&mutex);
    
    while ((get_cnt(c->prod) - get_cnt(c->cons)) >= BOUNDED_BUFFER_SIZE) {//This needs to be a different check.
      pthread_cond_wait(&empty, &mutex); //Condition should be "there's room"
    }
    // printf("out of while\n");
    Matrix * mat = GenMatrixRandom();  //Get matrix mode here!
    addTo_cnt(c->prodSum, SumMatrix(mat));
    
    put(mat);
    increment_cnt(c->prod);
    if (fill_ptr > 1) {
        pthread_cond_signal(&full);
    }
    printf("produced: %d\n", get_cnt(c->prod));
    pthread_mutex_unlock(&mutex);
  }
  return 0;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *counters)
{
  counters_t* c = (counters_t*)counters;
  int i;
  for (i = 0; i < loops; i++) 
  {
    pthread_mutex_lock(&mutex);
    Matrix *m1, *m2, *m3;
    while (fill_ptr == 0) 
      pthread_cond_wait(&full, &mutex); //Condition is "2 or more matrix in bb"
    m1 = get(); //Need to get two matrices! and multiply/ return them...
    increment_cnt(c->cons);
    addTo_cnt(c->conSum, SumMatrix(m1));
    m2 = get();
    increment_cnt(c->cons);
    m3 = MatrixMultiply(m1, m2);
    if(m3 == NULL) {
      FreeMatrix(m2);
      m2 = get();
      increment_cnt(c->cons);
      m3 = MatrixMultiply(m1, m2);
    }
    addTo_cnt(c->conSum, SumMatrix(m1));
    
    increment_cnt(c->mult);
    DisplayMatrix(m1, stdout);
    printf("    X\n");
    DisplayMatrix(m2, stdout);
    printf("    =\n");
    DisplayMatrix(m3, stdout);
    printf("\n");
    
    printf("consumed: %d\n", get_cnt(c->cons));
    FreeMatrix(m3);
    FreeMatrix(m2);
    FreeMatrix(m1);


    pthread_cond_signal(&empty); 
    pthread_mutex_unlock(&mutex);
}
  // pthread_cond_wait(&full, &mutex);
  printf("done\n");
  // pthread_exit(args);
  // pthread_join(pthread_self(), NULL);
  // printf("done\n");
  return 0;
}