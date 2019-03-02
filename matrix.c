/*
 *  Matrix routines
 *  Supports generation of random R x C matrices
 *  And operations on them
 * 
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Fall 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <time.h>
#include "matrix.h"
#include "pcmatrix.h"


// MATRIX ROUTINES
Matrix * AllocMatrix(int r, int c)
{
  Matrix * mat;
  mat = (Matrix *) malloc(sizeof(Matrix)); 
  int ** a;
  int i;
  a = (int**) malloc(sizeof(int *) * r);
  assert(a != 0);
  for (i = 0; i < r; i++)
  {
    a[i] = (int *) malloc(c * sizeof(int));
    assert(a[i] != 0);
  }
  mat->m=a;
  mat->rows=r;
  mat->cols=c;
  return mat;
}

void FreeMatrix(Matrix * mat)
{
  int r = mat->rows;
  //int c = mat->cols;
  int **a = mat->m;
  int i;
  for (i=0; i<r; i++)
  {
    free(a[i]);
  }
  free(a);
  free(mat);
}

void GenMatrix(Matrix * mat)
{
  int height = mat->rows;
  int width = mat->cols;
  int ** a = mat->m;
  int i, j;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      int * mm = a[i];
      if (MATRIX_MODE == 0)
        mm[j] = 1 + rand() % 10;
      else
        mm[j] = 1;
#if OUTPUT
      printf("matrix[%d][%d]=%d \n",i,j,mm[j]);
#endif
    }
  }
}

Matrix * GenMatrixRandom()
{
  int row;
  int col;
  if (MATRIX_MODE ==0)
  {
    row = 1 + rand() % 4;
    col = 1 + rand() % 4;
  }
  else
  {
    row = MATRIX_MODE;
    col = MATRIX_MODE;
  }
  Matrix * mat = AllocMatrix(row, col);
  GenMatrix(mat);
  return mat; 
}

Matrix * GenMatrixBySize(int row, int col)
{
  printf("Generate random matrix (RxC) = (%dx%d)\n",row,col);
  Matrix * mat = AllocMatrix(row, col);
  GenMatrix(mat);
  return mat; 
}

Matrix * MatrixMultiply(Matrix * m1, Matrix * m2)
{
  if ((m1==NULL) || (m2==NULL))
    printf("m1=%p  m2=%p!\n",m1,m2);
  int sum=0;
  if (m1->cols != m2->rows)
  {
    return NULL;
  }
  printf("MULTIPLY (%d x %d) BY (%d x %d):\n",m1->rows,m1->cols,m2->rows,m2->cols);
  Matrix * newmat = AllocMatrix(m1->rows, m2->cols);
  int ** nm = newmat->m;
  int ** ma1 = m1->m;
  int ** ma2 = m2->m;
  for (int c=0;c<newmat->rows;c++)
  {
    for (int d=0;d<newmat->cols;d++)
    {
      for (int k=0;k<m2->rows;k++)
      {
        sum = sum + ma1[c][k]*ma2[k][d]; 
      }
      nm[c][d] = sum;
      sum=0;
    }
  }
  return newmat;
}

void DisplayMatrix(Matrix * mat, FILE *stream)
{
  if ((mat == NULL) || (mat->m == NULL))
  { 
    printf("DisplayMatrix: EMPTY matrix\n");
    return;
  }
  int ** matrix = mat->m;
  int height = mat->rows;
  int width = mat->cols; 
  int y=0;
  int i, j;
  for (i=0; i<height; i++)
  {
    int *mm = matrix[i];
    fprintf(stream, "|");
    for (j=0; j<width; j++)
    {
      y=mm[j];
      if (j==0)
        fprintf(stream, "%3d",y);
      else
        fprintf(stream, " %3d",y);
    }
    fprintf(stream, "|\n");
  }
}


int AvgElement(Matrix * mat) // int ** matrix, const int height, const int width)
{
  int ** a = mat->m;
  int height = mat->rows;
  int width = mat->cols; 
  int x=0;
  int y=0;
  int ele=0;
  int i, j;
  for (i=0; i<height; i++)
    for (j=0; j<width; j++)
    {
      int *mm = a[i];
      y=mm[j];  
      x=x+y;
      ele++;
#if OUTPUT
      printf("[%d][%d]--%d x=%d ele=%d\n",i,j,mm[j],x,ele);
#endif
    }
  printf("x=%d ele=%d\n",x, ele);
  return x / ele; 
}

int SumMatrix(Matrix * mat) {
   int ** a = mat->m;
   int height = mat->rows;
   int width = mat->cols;
   int i =0;
   int j =0;
   int y =0;
   int total = 0;
   for (i = 0; i < height; i++) 
   {
      for (j = 0; j < width; j++)
      {
	  int *mm = a[i];
	  y=mm[j];
	  total = total+y;
      }
   }
   return total;
}



 
