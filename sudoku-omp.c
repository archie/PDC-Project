#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

int SIZE;
int l;

//long boards_processed[8];

FILE *inputMatrix;

typedef struct matrix {
  short **data;
  short **fixed;
} MATRIX;

struct list_el {
   MATRIX mat;
   short i, j;
   struct list_el *next;
};

typedef struct list_el item;

MATRIX solution;

item *head;
item *tail;

MATRIX read_matrix_with_spaces(char *filename) {
  int i,j;  
  MATRIX matrix;  
  int element_int;

  inputMatrix = fopen(filename, "rt");

  fscanf(inputMatrix, "%d", &element_int);
  l = element_int;
  SIZE = l*l;

  // allocate memory for matrix
  matrix.data = (short**)malloc(SIZE*sizeof(short*));  
  for (i=0;i<SIZE;i++)
    matrix.data[i] = (short*) malloc (SIZE * sizeof (short));

  matrix.fixed = (short**) malloc(SIZE * sizeof(short*));
  for (i=0;i<SIZE;i++)
    matrix.fixed[i] = (short*) malloc (SIZE * sizeof (short));
  
  // init
  for (i=0; i<SIZE; i++) {
    for (j=0; j<SIZE; j++) {     
      matrix.fixed[i][j] = 0;
    }
  }
  
  for(i = 0; i < SIZE; i++) {
    for(j = 0; j < SIZE; j++){
      fscanf(inputMatrix, "%d", &element_int);
      matrix.data[i][j] = element_int;
      if (matrix.data[i][j] != 0)
        matrix.fixed[i][j] = 1;
    }  
  }
  
  fclose(inputMatrix);
  
  return matrix;
}

void printMatrix(MATRIX *matrix) {
  int i,j;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%2d ", matrix->data[i][j]);
    }
    printf("\n");
  }
}

short permissible(MATRIX matrix, short i_line, short j_col) {

  short line, column;
  short value = matrix.data[i_line][j_col];

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
  short igroup = (i_line / l) * l;
  short jgroup = (j_col / l) * l;
  for (line = igroup; line < igroup+l; line++) {
    for (column = jgroup; column < jgroup+l; column++) {
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

void decreasePosition(MATRIX* matrix, short* iPointer, short* jPointer){
      do {
        if (*jPointer == 0 && *iPointer > 0) {
          *jPointer = SIZE - 1;
          (*iPointer)--;
        } else
          (*jPointer)--;
      } while (*jPointer >= 0 && (*matrix).fixed[*iPointer][*jPointer] == 1);
}

void increasePosition(MATRIX* matrix, short* iPointer, short* jPointer){
  
  do{
    if(*jPointer < SIZE-1)
      (*jPointer)++;
    else {
      *jPointer = 0;
      (*iPointer)++;
    }
  } while (*iPointer < SIZE && (*matrix).fixed[*iPointer][*jPointer]);
}

void freeListElement(item *node) {
  int i;
  for (i = 0; i < SIZE; i++) {
    free(node->mat.data[i]);
    free(node->mat.fixed[i]);
  }
  free(node->mat.data);
  free(node->mat.fixed);
  free(node);
}

item* createItem(MATRIX matrix, short i, short j){
  item * curr = (item *)malloc(sizeof(item));
  int m;
  short x, y;

  /* allocate memory for new copy */
  curr->mat.data = (short**)malloc(SIZE*sizeof(short*));
  for (m=0;m<SIZE;m++)
    curr->mat.data[m] = (short*) malloc (SIZE * sizeof (short));
  
  curr->mat.fixed = (short**) malloc(SIZE * sizeof(short*));
  for (m=0;m<SIZE;m++)
    curr->mat.fixed[m] = (short*) malloc (SIZE * sizeof (short));


  //copy matrix
  for(x = 0; x < SIZE; x++){
    for(y = 0; y < SIZE; y++){
      curr->mat.data[x][y] = matrix.data[x][y];
      curr->mat.fixed[x][y] = matrix.fixed[x][y]; 
    }
  }


  curr->i = i;
  curr->j = j;
  curr->next = NULL;
  
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


/* Initialize permissible matrix pool */
void initializePool2(MATRIX* matrix){

  short i = 0;
  short j = 0;

  if ((*matrix).fixed[i][j] == 1)
    increasePosition(matrix, &i, &j);

  short num;
  for(num = 0; num < SIZE; num++){
    ((*matrix).data[i][j])++;    
    if (permissible(*matrix, i, j) == 1) {
      item* newPath = createItem(*matrix, i, j);
      attachItem(newPath);
    } 
  }

}

/* Improved Initialize permissible matrix pool */
/* Improved Initialize permissible matrix pool */
void initializePool(MATRIX* matrix){

  short i = 0;
  short j = 0;

  if ((*matrix).fixed[i][j] == 1)
    increasePosition(matrix, &i, &j);

  short num=0;
  short valid_value=0;

  while( num<2*SIZE && i<SIZE ) {
    ((*matrix).data[i][j])++;    
    
    //adding the matrix to the pool only if the value is permissible
    if (matrix->data[i][j] <= SIZE && permissible(*matrix, i, j) == 1) {
      item* newPath = createItem(*matrix, i, j);
      attachItem(newPath);
      //printf("matrix %d added to pool\n",num);
      //printMatrix(matrix);
      //printf("\n");
      valid_value = matrix->data[i][j];
      num++;
    } else if(matrix->data[i][j] > SIZE) {
      if (valid_value != 0){
        //moving to next position if all values have been tried for this position
        matrix->data[i][j] = valid_value;
        increasePosition(matrix, &i, &j);
        valid_value = 0;
      } else {
        //no valid value was found in this position, impossible to move to next position
        break;
      }
    }
  /*end of changes done by wasif*/	
 }
   
//printf("\nCreated %d initial boards.\n", num);

}

short bf_pool(MATRIX matrix) {

  head = NULL;
  tail = NULL;

  initializePool(&matrix);


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

  short found = 0;
  short i, j;
  item* current;
  int level;

#pragma omp parallel shared(found) private(i,j, current, level)
{

  #pragma omp critical (pool)
  current = removeItem();
  
  while(current != NULL && found == 0){
    //boards_processed[omp_get_thread_num()]++;
    
    MATRIX currMat = current->mat;

    i = current->i;
    j = current->j;

    increasePosition(&currMat, &i, &j);

    level = 1;

    while (level > 0 && i < SIZE && found == 0) {
      if (currMat.data[i][j] < SIZE) {    
        // increase cell value, and check if
        // new value is permissible
        currMat.data[i][j]++;

        if (permissible(currMat, i, j) == 1) {
          increasePosition(&currMat, &i, &j);
          level++;
        }
      } else {
        // goes back to the previous non-fixed cell

        currMat.data[i][j] = 0;
        decreasePosition(&currMat, &i, &j);
        level--;
      } // end else

    } // end while

    if(i == SIZE){
      found = 1;
      solution = currMat;
      
      continue;
    }

    free(current);

    #pragma omp critical (pool)
    current = removeItem();

   }

}  /* End of parallel block */ 

  return found;
}

int main(int argc, char* argv[]) {

  if(argv[1] == NULL) {
    printf("\n\n Usage: %s filename\n\n", argv[0]);
    exit(1);
  } 


  MATRIX m = read_matrix_with_spaces(argv[1]);

  int i,j;

//  printf("\nInput Matrix:\n");
//  printMatrix(&m);

  short hasSolution = bf_pool(m);
  
  if(hasSolution == 0){
    printf("No result!\n");
    return 1;
  }
  
  printMatrix(&solution);

  item* node = head;

  while (node != NULL) {
    item* next = node->next;
    freeListElement(node);
    node = next;
  }
 
// Debugging info 
//  printf("\nHas %d threads\n", omp_get_max_threads());
//  int thread;
//  for (thread = 0; thread < omp_get_max_threads(); thread++) {
//    printf("Thread %d: %ld\n", thread, boards_processed[thread]);
//  }

  return 0;

}
