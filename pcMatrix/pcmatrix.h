/*
 *  pcmatrix header
 *  Defines constants for pcMatrix program
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

// Number of worker threads - NUMWORK producers, NUMWORK consumers
#define NUMWORK 4

// Constant for enabling and disabling DEBUG output
#define OUTPUT 0

// Size of the buffer ARRAY  (see ch. 30, section 2, producer/consumer)
#define MAX 200
int BOUNDED_BUFFER_SIZE;

// Number of matrices to produce/consume
#define LOOPS 1200
int NUMBER_OF_MATRICES;

// MATRIX MODE FLAG
// mode 0 - Generate random matricies
// mode 1-n - Specifies a fixed number of rows and cols with matrix elements of 1
#define DEFAULT_MATRIX_MODE 0
int MATRIX_MODE;



