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
int buf_size = 0;
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
  pthread_mutex_lock(&buflock);
  if (mat != NULL) {
    // use_ptr = fill_ptr; //Lock issue here, need to update the use pointer to use a made matrix.
    // GenMatrix(buffer[fill_ptr]);
    printf("buff: %d \n", buf_size);
    if (buf_size < MAX) {
      buffer[fill_ptr] = mat;
      fill_ptr = (fill_ptr + 1) % MAX;  
      buf_size++;
    }    
    printf("fill: %d \n", fill_ptr);
    printf("use: %d \n", use_ptr);
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
    use_ptr = (use_ptr + 1) % MAX;
    buf_size--;
  }
  pthread_mutex_unlock(&buflock);
  return (tmp != NULL) ? tmp : 0;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *prodCount)
{
    int i;
    for (i = 0; i < loops; i++) {
      pthread_mutex_lock(&mutex);
      while (buf_size == MAX) {//This needs to be a different check.
        pthread_cond_wait(&empty, &mutex); //Condition should be "there's room"
      }
      // printf("out of while\n");
      Matrix * mat = GenMatrixRandom();  //Get matrix mode here!
      put(mat);
      increment_cnt(prodCount);
      // if (fill_ptr > 1) {
      //    pthread_cond_signal(&full);
      // }
      pthread_cond_signal(&full);
      pthread_mutex_unlock(&mutex);
      printf("produced: %d\n", get_cnt(prodCount));
    }
    return 0;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *conCount)
{
  printf("step1\n");
  int i;
   for (i = 0; i < loops; i++) 
  {
    pthread_mutex_lock(&mutex);
    // printf("step2\n");
    while (buf_size == 0) 
      pthread_cond_wait(&full, &mutex); //Condition is "2 or more matrix in bb"
    // printf("step3\n");
    Matrix * m1 = get(); //Need to get two matrices! and multiply/ return them...
    // printf("step4\n");
    increment_cnt(conCount);
    while (buf_size == 0) 
      pthread_cond_wait(&full, &mutex); //Condition is "2 or more matrix in bb"
    Matrix * m2 = get();
    increment_cnt(conCount);

    printf("fill: %d \n", fill_ptr);
    printf("use: %d \n", use_ptr);
    printf("m1: %d \n", m1->rows);
    printf("m2: %d \n", m2->rows);

    Matrix * m3 = MatrixMultiply(m1, m2);

    while (m3 == NULL) {
      FreeMatrix(m2);
      while (buf_size == 0) 
        pthread_cond_wait(&full, &mutex); 
      m2 = get();
      increment_cnt(conCount);
      m3 = MatrixMultiply(m1, m2);
    }

    DisplayMatrix(m1, stdout);
    printf("    X\n");
    DisplayMatrix(m2, stdout);
    printf("    =\n");
    DisplayMatrix(m3, stdout);
    printf("\n");

    FreeMatrix(m3);
    FreeMatrix(m2);
    FreeMatrix(m1);

//     if(m3 == NULL) {
//       printf("step6\n");
//       printf("step7 \n");
//       FreeMatrix(m2);
//       printf("step7 \n");
//       m2 = get();
//       if (&m2 == 0) {
//  printf("step7 \n");
//   printf("step7 \n");
//    printf("step7 \n");

//       }
     
//       increment_cnt(conCount);
//     } else {

//     }
     
//     printf("consumed: %d\n", get_cnt(conCount));

 
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);

  }
  return 0;
}