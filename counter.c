/*
 *  Signal example 
 *  Based on Operating Systems: Three Easy Pieces by R. Arpaci-Dusseau and A. Arpaci-Dusseau
 * 
 *  This example uses locks and a condition to synchronize a child thread and
 *  the parent.  The child generates matrices and the parent computes the
 *  average when the matrix is ready for processing.
 *
 *  Wes J. Lloyd
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

// Include libraries required for this module only
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "counter.h"

// SYNCHRONIZED COUNTER METHOD IMPLEMENTATION
// Based on Three Easy Pieces

// Initiates a counter. sets initial value to 0.
void init_cnt(counter_t *c)  {
  c->value = 0;
  pthread_mutex_init(&c->lock, NULL);
}

// Increments the counter by 1.
void increment_cnt(counter_t *c)  {
  pthread_mutex_lock(&c->lock);
  c->value++;
  pthread_mutex_unlock(&c->lock);
}

// INcrements the counter b the value passed.
void addTo_cnt(counter_t *c, int n) {
  pthread_mutex_lock(&c->lock);
  c->value += n;
  pthread_mutex_unlock(&c->lock);
}

// returns an int of the value stored in the counter.
int get_cnt(counter_t *c)  {
  pthread_mutex_lock(&c->lock);
  int rc = c->value;
  pthread_mutex_unlock(&c->lock);
  return rc;
}


