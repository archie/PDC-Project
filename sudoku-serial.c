#include <stdlib.h>
#include <stdio.h>

#define SIZE 9

FILE *inputMatrix;

typedef struct matrix {
  int data[SIZE][SIZE];
} MATRIX;

MATRIX read_matrix(void) {
  MATRIX matrix;
  int i,j;
  char line[SIZE];
  inputMatrix = fopen("proper1.txt", "rt");
  
  i = 0;
  while (fgets(line, SIZE+2, inputMatrix)) {
    for (j = 0; j < SIZE; j++) {
      matrix.data[i][j] = line[j] - '0';     
    }
    i++;
  }
  
  fclose(inputMatrix);

  return matrix;
}


int main(int argc, char* argv[]) {
  MATRIX m = read_matrix();
  int i,j;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d", m.data[i][j]);
    }
    printf("\n");
  }
}
