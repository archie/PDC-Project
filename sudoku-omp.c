#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define SIZE 9

FILE *inputMatrix;

typedef struct matrix {
  int data[SIZE][SIZE];
  int fixed[SIZE][SIZE];
} MATRIX;

struct list_el {
   MATRIX mat;
   int i, j;
   struct list_el *next;
};

typedef struct list_el item;

item *head;
item *tail;

MATRIX finalResult;

MATRIX read_matrix_with_spaces(char *filename) {
  MATRIX matrix;
  int i,j;
  char line[SIZE+1];
  char element[1];
  int l;
  inputMatrix = fopen(filename, "rt");

  // init
  for (i=0; i < SIZE; i++)
    for (j=0; j<SIZE; j++) 
      matrix.fixed[i][j] = 0;
	
  fscanf(inputMatrix, "%s", element);
	l = element[0]-'0';
	printf("\nl=%d",l);

  for(i = 0; i < SIZE; i++)
    for(j = 0; j < SIZE; j++){
	    fscanf(inputMatrix, "%s", element);
	    matrix.data[i][j] = element[0] - '0';
      if (matrix.data[i][j] != 0)
        matrix.fixed[i][j] = 1;
    }
  
  fclose(inputMatrix);

  return matrix;
}

int permissible(MATRIX matrix, int i_line, int j_col) {

  int line, column;
  int value = matrix.data[i_line][j_col];

  // check same column
  for (line = 0; line < SIZE; line++) {
    if (matrix.data[line][j_col] == 0)
      continue;

    if ((i_line != line) && 
        (matrix.data[line][j_col] == value)) 
      return 0;
  }

  // check same line
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
  if (*jPointer == 0 && *iPointer > 0) {
    *jPointer = SIZE - 1;
    (*iPointer)--;
  } else
    (*jPointer)--;
}

void increasePosition(int* iPointer, int* jPointer){
  if(*jPointer < SIZE-1)
    (*jPointer)++;
  else {
    *jPointer = 0;
    (*iPointer)++;
  }
}

MATRIX bruteforce(MATRIX matrix) {
    
  int i, j;
  i = 0;
  j = 0;

  while (i < SIZE) {

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


item* createItem(MATRIX matrix, int i, int j){
  item * curr = (item *)malloc(sizeof(item));

  

  int x, y;
  //copy matrix
  for(x = 0; x < SIZE; x++){
    for(y = 0; y < SIZE; y++){
      curr->mat.data[x][y] = matrix.data[x][y];
      curr->mat.fixed[x][y] = matrix.fixed[x][y]; 
      //      printf("%d ", matrix.data[x][y]);
    }
    //printf("\n");
  }


  curr->i = i;
  curr->j = j;
  curr->next = NULL;



  // usleep(100000);
  return curr;
}

void attachItem(item* newItem){

  if(head == NULL){
    head = newItem;
    tail = newItem;
  } else {
    tail->next = newItem;
    tail = newItem;
  }
}

item* removeItem(){
  item* result = NULL;
  if(head != NULL){
    result = head;
    head = result->next;
    if(head == NULL){
      tail = NULL;
    }
  }
  return result;
}


MATRIX* bf_repository(MATRIX matrix) {

  head = NULL;
  tail = NULL;

  int i = 0;
  int j = 0;

  /* Initialize permissible matrix repository */
  // (should be done only by the master thread)

  while (matrix.fixed[i][j] == 1)
    increasePosition(&i, &j);

  int num;
  for(num = 0; num < SIZE; num++){
    matrix.data[i][j]++;    
    if (permissible(matrix, i, j) == 1) {
      item* newPath = createItem(matrix, i, j);
      attachItem(newPath);
    } 
  }

  /* End initialization */

  /* Begin of parallel block
      - result is a global variable
      - current, i and j are private variables
      - access to methods removeItem 
        and attachItem should be exclusive
  */

  /* Remove matrix from repository, and
     move to the next non-fixed position,
     adding permissible matrices to the
     repository for the next iteration 
   */
  item *current;
  MATRIX* result = NULL;


#pragma omp parallel shared(result) private(num, i,j, current) 
{

#pragma omp critical (pool) 
{
    current = removeItem();
}

  while(current != NULL && result == NULL){
    MATRIX currMat = current->mat;
    i = current->i;
    j = current->j;

    do{
      increasePosition(&i, &j);
    } while (i < SIZE && currMat.fixed[i][j] == 1);

    if(i == SIZE){
      finalResult = currMat;
      result = &currMat;
      continue;
    }


    for(num = 0; num < SIZE; num++){
      currMat.data[i][j]++;
      if (permissible(currMat, i, j) == 1) {
        item* newPath = createItem(currMat, i, j);
          
#pragma omp critical (pool) 
        attachItem(newPath);
        
      }
    } 
    free(current);
    
#pragma omp critical (pool)
    current = removeItem();
    
  } /* end while */
} /* End of parallel block */ 

  return result;
}

int main(int argc, char* argv[]) {

  if(argv[1] == NULL) {
    printf("\n\n Usage: %s filename\n\n", argv[0]);
    exit(1);
  } 
MATRIX m = read_matrix_with_spaces(argv[1]);
  int i,j;

  printf("\nInput Matrix:\n");
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d ", m.data[i][j]);
    }
    printf("\n");
  }

  printf("\n\n");

  MATRIX* result = bf_repository(m);
  
  if(result == NULL){
    printf("No result!\n");
    return;
  }
  
  MATRIX solved = *result;
  //MATRIX solved = bruteforce(m);
  
  printf("Result Matrix:\n");
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d ", finalResult.data[i][j]);
    }
    printf("\n");
  }  
  
  
}
