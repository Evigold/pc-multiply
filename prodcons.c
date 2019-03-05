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
int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;

// Define Locks and Condition variables here
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Producer consumer data structures
ProdConsStats pcStats;


// Bounded buffer bigmatrix defined in prodcons.h
// Matrix ** bigmatrix;
// bigmatrix is inmitialized in main... 

// Bounded buffer put() get()
int put(Matrix * mat)
{
  if (&mat != NULL) {
    buffer[fill_ptr] = mat;
    use_ptr = fill_ptr; //Lock issue here, need to update the use pointer to use a made matrix.
    GenMatrix(buffer[fill_ptr]);
    fill_ptr = (fill_ptr + 1) % MAX;  
    return 1;
  }
  return 0;
}

Matrix * get() 
{
  int tmp = buffer[use_ptr];
  use_ptr = (use_ptr - 1) % MAX;
  if (use_ptr < 2) {
    pthread_cond_signal(&empty); //Different condition? signal that there is nothing in buffer now.
  }
  if (&tmp != NULL) {
    return tmp;
  } else {
    return 0;
  }
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
    int i;
    for (i = 0; i < loops; i++) {
      pthread_mutex_lock(&mutex);
      while (count == MAX) 
        pthread_cond_wait(&empty, &mutex); //Condition should be "there's room"
      Matrix * mat = AllocMatrix(2, 2);  //Get matrix mode here!
      put(mat);
      increment_cnt(pcStats.prodCount);
    }
    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&mutex);
    return 0;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  int i;
  for (i = 0; i < loops; i++) 
  {
    pthread_mutex_lock(&mutex);
    while (count == 0) 
    {
      pthread_cond_wait(&fill, &mutex); //Condition is "2 or more matrix in bb"
      Matrix * m1 = get(); //Need to get two matrices! and multiply/ return them...
      Matrix * m2 = get();
      Matrix * m3 = MatrixMultiply(m1, m2);
      if (m3 != NULL){
        DisplayMatrix(m1, stdout);
        printf("    X\n");
        DisplayMatrix(m2, stdout);
        printf("    =\n");
        DisplayMatrix(m3, stdout);
        printf("\n");
      }
      FreeMatrix(m3);
      FreeMatrix(m2);
      FreeMatrix(m1);
      increment_cnt(pcStats.conCount);
      increment_cnt(pcStats.conCount);
    }
    int tmp = get();
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    printf("%d\n", tmp);
  }

  return 0;
}

