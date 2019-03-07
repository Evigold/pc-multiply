/*
 *  counter header
 *  Function prototypes, data, and constants for synchronized counter module
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

// SYNCHRONIZED COUNTER

// counter structures 
typedef struct __counter_t {
  int value;
  pthread_mutex_t  lock;
} counter_t;

// Counters struct that contains all the different statistics counters needed for the program.
typedef struct __counters_t {
  counter_t * prodCount;
  counter_t * consCount;
  counter_t * prodSum;
  counter_t * consSum;
  counter_t * multCount;
} counters_t;

// counter methods
void init_cnt(counter_t *c);
void increment_cnt(counter_t *c);
void addTo_cnt(counter_t *c, int n);
int get_cnt(counter_t *c);



