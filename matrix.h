/*
 *  Matrix header
 *  Function prototypes, data, and constants for matrix module
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

#define ROW 5
#define COL 5

typedef struct matrix {
  int rows;
  int cols;
  int ** m;
} Matrix;

int theseed;

// MATRIX ROUTINES
Matrix * AllocMatrix(int r, int c);
void FreeMatrix(Matrix * mat);
void GenMatrix(Matrix * mat);
Matrix * GenMatrixRandom();
int AvgElement(Matrix * mat);
int SumMatrix(Matrix * mat);
Matrix * MatrixMultiply(Matrix * m1, Matrix * m2);
void DisplayMatrix(Matrix * mat, FILE *stream);
Matrix * GenMatrixBySize(int row, int col);
