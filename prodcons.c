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

// Define buffer used to store the produced matrixies and for the consumer to retrieve from.
Matrix * buffer[MAX];
// Define variables that help
int fill_ptr = 0;
int use_ptr = 0;
int buf_size = 0;

// Define Locks and Condition variables here
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; //BB is empty.
pthread_cond_t full = PTHREAD_COND_INITIALIZER;  //BB is full.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buflock = PTHREAD_MUTEX_INITIALIZER;  //Move a lock to get/put methods?


// Bounded buffer put() get()
int put(Matrix * mat)
{
  pthread_mutex_lock(&buflock);
  if (mat != NULL) {
    if (buf_size < MAX) {
      buffer[fill_ptr] = mat;
      use_ptr = fill_ptr;
      fill_ptr = (fill_ptr + 1) % MAX;  
      buf_size++;
    }    
  }
  pthread_mutex_unlock(&buflock);
  return 0;
}

Matrix * get() 
{

  Matrix * tmp;
  pthread_mutex_lock(&buflock);
  if (buf_size > 0) {
    tmp = buffer[use_ptr];
    fill_ptr = use_ptr;
    use_ptr = (use_ptr - 1) % MAX;
    buf_size--;
  }
  pthread_mutex_unlock(&buflock);
  return (tmp != NULL) ? tmp : 0;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *args)
{
  counters_t *stats = (counters_t *)args;
  while(get_cnt(stats->prodCount) < NUMBER_OF_MATRICES) {
    pthread_mutex_lock(&mutex);
    // Check if buffer is full, if so, waits.
    while (buf_size == MAX) {
      // Checks if job has been finished while waiting. If so, ends thread(returns).
      if(get_cnt(stats->prodCount) >= NUMBER_OF_MATRICES) {
        pthread_cond_signal(&full);
        pthread_mutex_unlock  (&mutex);
        goto end;
      }pthread_cond_wait(&empty, &mutex); //Condition should be "there's room"
    }
    
    Matrix * mat = GenMatrixRandom();  //Get matrix mode here!
    addTo_cnt(stats->prodSum, SumMatrix(mat));
    put(mat);
    increment_cnt(stats->prodCount);
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mutex);
  }
  end:return 0;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *args)
{
  counters_t *stats = (counters_t *)args;
  while(get_cnt(stats->consCount) < NUMBER_OF_MATRICES) {
    pthread_mutex_lock(&mutex);
    // Checks to see if buffer is empty. if so, waits.
    while (buf_size == 0) { 
      // Checks if job has been finished while waiting. If so, ends thread(returns).
      if(get_cnt(stats->consCount) >= NUMBER_OF_MATRICES) {
        pthread_cond_signal(&full);
        pthread_mutex_unlock  (&mutex);
        goto end;
      }
      pthread_cond_wait(&full, &mutex); 
    }
    
    Matrix * m1 = get();
    increment_cnt(stats->consCount);
    addTo_cnt(stats->consSum, SumMatrix(m1));
    // Checks to see if buffer is empty. if so, waits.
    while (buf_size == 0) { 
      // Checks if job has been finished while waiting. If so, ends thread(returns).
      if(get_cnt(stats->consCount) >= NUMBER_OF_MATRICES || m1 == NULL) {
        pthread_cond_signal(&full);
        pthread_mutex_unlock  (&mutex);
        goto end;
      }
      pthread_cond_wait(&full, &mutex); 
    }
    
    Matrix * m2 = NULL;
    Matrix * m3;
    // While loop that keeps on switching m2 matrix until it finds one compatible with m1
    // to multiply.
    do {
      // Checks to see if buffer is empty. if so, waits.
      while (buf_size == 0) { 
      // Checks if job has been finished while waiting. If so, ends thread(returns).
        if(get_cnt(stats->consCount) >= NUMBER_OF_MATRICES || m2 == NULL) {
          pthread_cond_signal(&full);
          pthread_mutex_unlock  (&mutex);
          goto end;
        }
        pthread_cond_wait(&full, &mutex); 
      }
      if(m2 != NULL)  FreeMatrix(m2);
      m2 = get();
      increment_cnt(stats->consCount);
      addTo_cnt(stats->consSum, SumMatrix(m2));
      m3 = MatrixMultiply(m1, m2);
    } while (m3 == NULL);
    
    increment_cnt(stats->multCount);
    DisplayMatrix(m1, stdout);
    printf("    X\n");
    DisplayMatrix(m2, stdout);
    printf("    =\n");
    DisplayMatrix(m3, stdout);
    printf("\n");

    FreeMatrix(m3);
    FreeMatrix(m2);
    FreeMatrix(m1);

    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
  }
  end:return 0;
}