#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

//#define SIZE 9
int SIZE;
#define STEP 80

long boards_processed[8]; // assuming we will never run this on more than 8 cores

FILE *inputMatrix;

typedef struct matrix {
  //  short data[SIZE][SIZE];
  //  short fixed[SIZE][SIZE];
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
 
  inputMatrix = fopen(filename, "rt");

  int element_int;
  int l;

  fscanf(inputMatrix, "%d", &element_int);
  l = element_int;
  SIZE = l*l;
  printf("\nl=%d\tSIZE=%d",l, SIZE);


 int i,j;  
  MATRIX matrix;  
matrix.data = (short**)malloc(SIZE*sizeof(short*));

 for (i=0;i<SIZE;i++)
  matrix.data[i] = (short*) malloc (SIZE * sizeof (short));

matrix.fixed = (short**) malloc(SIZE * sizeof(short*));

 for (i=0;i<SIZE;i++)
  matrix.fixed[i] = (short*) malloc (SIZE * sizeof (short));

  // init
 for (i=0; i<SIZE; i++)
   for (j=0; j<SIZE; j++) {     
     matrix.fixed[i][j] = 0;
   }

  for(i = 0; i < SIZE; i++)
    for(j = 0; j < SIZE; j++){
      fscanf(inputMatrix, "%d", &element_int);
      matrix.data[i][j] = element_int;
      if (matrix.data[i][j] != 0)
        matrix.fixed[i][j] = 1;
    }

  fclose(inputMatrix);

  return matrix;
}


/*
MATRIX read_matrix_with_spaces(char *filename) {
  MATRIX matrix;
  short i,j;
  char line[SIZE+1];
  char element[1];
  short l;
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
*/

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
  short igroup = (i_line / 3) * 3;
  short jgroup = (j_col / 3) * 3;
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

//MATRIX bruteforce(MATRIX matrix) {
//    
//  int i, j;
//  i = 0;
//  j = 0;
//
//  while (i < SIZE) {
//
//    if (matrix.fixed[i][j] == 1)
//      // fixed cell
//      increasePosition(&i, &j);
//    else if (matrix.data[i][j] < SIZE) {    
//        // increase cell value, and check if
//        // new value is permissible
//
//        matrix.data[i][j]++;
//        if (permissible(matrix, i, j) == 1) {
//          increasePosition(&i, &j);
//        }
//
//    } else {
//      // tried all the values for this cell
//      // goes back to the previous non-fixed cell
//
//      matrix.data[i][j] = 0;
//
//      do {
//        decreasePosition(&i, &j);
//      } while (matrix.fixed[i][j] == 1);
//
//    } // end else
//
//  } // end while
//
//  return matrix;
//} // end bruteforce


item* createItem(MATRIX matrix, short i, short j){
  item * curr = (item *)malloc(sizeof(item));
  int m;

  curr->mat.data = (short**)malloc(SIZE*sizeof(short*));
 for (m=0;m<SIZE;m++)
   curr->mat.data[m] = (short*) malloc (SIZE * sizeof (short));
 
 curr->mat.fixed = (short**) malloc(SIZE * sizeof(short*));
 for (m=0;m<SIZE;m++)
   curr->mat.fixed[m] = (short*) malloc (SIZE * sizeof (short));
 
 

  short x, y;
  //copy matrix
  //printf("New possibility:\n");
  for(x = 0; x < SIZE; x++){
    for(y = 0; y < SIZE; y++){
      curr->mat.data[x][y] = matrix.data[x][y];
      curr->mat.fixed[x][y] = matrix.fixed[x][y]; 
      //printf("%d ", matrix.data[x][y]);
    }
    //printf("\n");
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
void initializePool(MATRIX* matrix){

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
  item* current;
  short i, j, num;

#pragma omp parallel shared(found) private(i,j, current) 
{

  #pragma omp critical (pool)
  current = removeItem();
  while(current != NULL && found == 0){

    boards_processed[omp_get_thread_num()]++;
    MATRIX currMat = current->mat;

    i = current->i;
    j = current->j;

    increasePosition(&currMat, &i, &j);

    int level = 1;

    while (level > 0 && i < SIZE && found == 0) {
      if (level <= STEP && currMat.data[i][j] < SIZE) {    
          // increase cell value, and check if
          // new value is permissible
          currMat.data[i][j]++;

          if (permissible(currMat, i, j) == 1) {
            if(level < STEP) {
              increasePosition(&currMat, &i, &j);
              level++;
            } else {
              item* newPath = createItem(currMat, i, j);
              #pragma omp critical (pool)              
              attachItem(newPath);
              //sleep(10);
            }
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
   } //end while

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

  printf("\nInput Matrix:\n");
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d ", m.data[i][j]);
    }
    printf("\n");
  }

  printf("\n\n");

  int hasSolution = bf_pool(m);
  
  if(hasSolution == 0){
    printf("No result!\n");
    return 1;
  }
  
  printf("Result Matrix:\n");
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      printf("%d ", solution.data[i][j]);
    }
    printf("\n");
  }  
  
  
  printf("\nHas %d threads\n", omp_get_max_threads());
  int thread;
  for (thread = 0; thread < omp_get_max_threads(); thread++) {
    printf("Thread %d: %ld\n", thread, boards_processed[thread]);
  }

  return 0;
}
