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

typedef struct __counters_t {
  counter_t * prod;
  counter_t * cons;
  counter_t * conSum;
  counter_t * prodSum;
  counter_t * mult;
} counters_t;

// counter methods
void init_cnt(counter_t *c);
void init_cnts(counters_t * counters);
void increment_cnt(counter_t *c);
void addTo_cnt(counter_t *c, int a);
int get_cnt(counter_t *c);



