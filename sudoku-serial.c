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
  char line[SIZE];
  inputMatrix = fopen(filename, "rt");

  // init
  for (i=0; i < SIZE; i++)
    for (j=0; j<SIZE; j++) 
      matrix.fixed[i][j] = 0;

  i = 0;
  while (fgets(line, SIZE+2, inputMatrix)) {
    for (j = 0; j < SIZE; j++) {
      matrix.data[i][j] = line[j] - '0';
      if (matrix.data[i][j] != 0) 
        matrix.fixed[i][j] = 1;
    }
    i++;
  }
  
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
  
  int igroup = (i_line / 3) * 3;
  int jgroup = (j_col / 3) * 3;
  for (line = igroup; line < igroup+2; line++) {
    for (column = jgroup; column < jgroup+2; column++) {
      if (matrix.data[line][column] == 0)
        continue;

      if ((i_line != line) &&
          (j_col != column) &&
          (matrix.data[line][column] == value)) {
        return 0; //TODO: wiki optimise
      }
    }
  }

  return 1;
}


MATRIX bruteforce(MATRIX matrix) {

  //init
    
  int i, j;
  i = 0;
  j = 0;

  while (j < SIZE) {

    if (matrix.fixed[i][j] == 0) {
      if (matrix.data[i][j] == 9) {
        matrix.data[i][j] = 0;

        if (i == 0 && j > 0) {
          i = SIZE - 1;
          j--;
        } else 
          i--;        

        while (matrix.fixed[i][j] == 1) {
          if(i == 0 && j > 0) {
            i = SIZE-1;
            j--;
          } else 
            i--;
        }
        
        if (i == 0 && j > 0) {
          i = SIZE - 1;
          j--;
        } else 
          i--;
      } else {

      matrix.data[i][j]++;
      if (permissible(matrix, i, j) == 0) {
        //matrix.data[i][j]--;
        if (i == 0 && j > 0) {
          i = SIZE - 1;
          j--;
        } 
        else
          i--;
      }  else {
         printf("I: %d, J: %d, M[I,J]: %d\n", i, j, matrix.data[i][j]);

      }
     }
    }

    if(i < SIZE-1)
      i++;
    else {
      i = 0;
      j++;
    }
  }

  return matrix;

  // for each empty cell insert t='1'
  // try if t is valid in this position (line, column, quandrant)
  // if not increment t by 1
}


int main(int argc, char* argv[]) {
  MATRIX m = read_matrix(argv[1]);
  int i,j;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d", m.data[i][j]);
    }
    printf("\n");
  }

  MATRIX solved = bruteforce(m);
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d", solved.data[i][j]);
    }
    printf("\n");
  }  
  
}
