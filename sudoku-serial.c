#include <stdlib.h>
#include <stdio.h>

#define SIZE 9

FILE *inputMatrix;

typedef struct matrix {
  int data[SIZE][SIZE];
  int fixed[SIZE][SIZE];
} MATRIX;

MATRIX read_matrix(char *filename) {
  MATRIX matrix;
  int i,j;
  char line[SIZE+1];
  inputMatrix = fopen(filename, "rt");

  // init
  for (i=0; i < SIZE; i++)
    for (j=0; j<SIZE; j++) 
      matrix.fixed[i][j] = 0;

  i = 0;
  while (fgets(line, SIZE+2 , inputMatrix)) {
    for (j = 0; j < SIZE; j++) {
      matrix.data[i][j] = line[j] - '0';
      if (matrix.data[i][j] != 0) 
        matrix.fixed[i][j] = 1;
      //printf("%d",matrix.data[i][j]);
    }

    i++;
  }
  //  printf("\n\n");
  /*int q,r;
 for (q = 0; q < SIZE; q++) {
   for (r = 0; r < SIZE; r++) {
      printf("%d", matrix.data[q][r]);
    }
    printf("\n");
  }
  printf("\n");  */
  fclose(inputMatrix);

  return matrix;
}


int permissible(MATRIX matrix, int i_line, int j_col) {

  int line, column;
  int value = matrix.data[i_line][j_col];

  // check line
  for (line = 0; line < SIZE; line++) {
    if (matrix.data[line][j_col] == 0)
      continue;

    if ((i_line != line) && 
        (matrix.data[line][j_col] == value)) 
      return 0;
  }

  // check column
  for (column = 0; column < SIZE; column++) {
    if (matrix.data[i_line][column] == 0)
      continue;

    if (j_col != column && matrix.data[i_line][column] == value)
      return 0;
  }
  
  // check group
  int igroup = (i_line / 3) * 3;
  int jgroup = (j_col / 3) * 3;
  for (line = igroup; line < igroup+2; line++) {
    for (column = jgroup; column < jgroup+2; column++) {
      if (matrix.data[line][column] == 0)
        continue;

      if ((i_line != line) &&
          (j_col != column) &&
          (matrix.data[line][column] == value)) {
        return 0;
      }
    }
  }

  return 1;
}

void decreasePosition(int* iPointer, int* jPointer){
  if (*iPointer == 0 && *jPointer > 0) {
    *iPointer = SIZE - 1;
    (*jPointer)--;
  } else
    (*iPointer)--;
}

void increasePosition(int* iPointer, int* jPointer){
  if(*iPointer < SIZE-1)
    (*iPointer)++;
  else {
    *iPointer = 0;
    (*jPointer)++;
  }
}

MATRIX bruteforce(MATRIX matrix) {
    
  int i, j;
  i = 0;
  j = 0;

  while (j < SIZE) {

    if (matrix.fixed[i][j] == 1)
      // fixed cell
      increasePosition(&i, &j);
    else if (matrix.data[i][j] < SIZE) {    
        // increase cell value, and check if
        // new value is permissible

        matrix.data[i][j]++;
        if (permissible(matrix, i, j) == 1) {
          increasePosition(&i, &j);
        }

    } else {
      // tried all the values for this cell
      // goes back to the previous non-fixed cell

      matrix.data[i][j] = 0;

      do {
        decreasePosition(&i, &j);
      } while (matrix.fixed[i][j] == 1);

    } // end else

  } // end while

  return matrix;
} // end bruteforce


int main(int argc, char* argv[]) {
  MATRIX m = read_matrix(argv[1]);
  int i,j;

  printf("\nInput Matrix:\n");
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d", m.data[i][j]);
    }
    printf("\n");
  }

    printf("\n\n");

  printf("Result Matrix:\n");
  MATRIX solved = bruteforce(m);
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d", solved.data[i][j]);
    }
    printf("\n");
  }  
  
}
