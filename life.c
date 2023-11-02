/*
Implementation of Conway's game of life!
The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970.

The board is made up of an m x n grid of cells, where each cell has an initial state: live (represented by a 1) or dead (represented by a 0). 
Each cell interacts with its eight neighbors (horizontal, vertical, diagonal) using the following four rules:

Any live cell with fewer than two live neighbors dies as if caused by under-population.
Any live cell with two or three live neighbors lives on to the next generation.
Any live cell with more than three live neighbors dies, as if by over-population.
Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
The next state is created by applying the above rules simultaneously to every cell in the current state.

Author: Mantha Sai Gopal
class: I M.Tech(CS)
Reg.No: 23358
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#define ROWS 1500
#define COLS 1500
#define DELAY 50000

// create the board
int **createBoard(){
    int** arr;
    arr = (int **)malloc(ROWS * sizeof(int *));
    for (int i = 0; i < ROWS; i++) {
        arr[i] = (int *) malloc(COLS * sizeof(int));
    }
    return arr;
}

// Initialize the board
void initialize(int** arr){
    #pragma omp parallel for collapse(2) if(ROWS > 20 && COLS > 20)
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            int randomNum = rand() % 2;
            arr[i][j] = randomNum;
        }
    }
}

// print the board
void printBoard(int **arr) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            
            if(arr[i][j] == 1){
                printf("*");
            }
            else{
                printf(".");
            }
            
            // printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

// check neighbors
int checkNeighbours(int **arr, int i, int j){
    int count = 0, row, col;

    #pragma omp parallel for collapse(2) if(ROWS > 20 && COLS > 20)
    for(int row_shift = -1; row_shift <= 1; row_shift++){
        for(int col_shift = -1; col_shift <= 1; col_shift++){
            row = i + row_shift;
            col = j + col_shift;
            if((row_shift != 0 || col_shift != 0) && 0 <= row && row < ROWS && 0 <= col && col < COLS){
                count += arr[row][col];
            }
        }
    }
    return count; 
}

// find next state
void nextState(int** arr) {
    int** tempArr = createBoard(); // Create a temporary matrix
    #pragma omp parallel for collapse(2) if(ROWS > 20 && COLS > 20)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int count = checkNeighbours(arr, i, j);
            int current_value = arr[i][j];
            int new_value = current_value;

            if (current_value == 1) {
                if (count < 2 || count > 3) {
                    new_value = 0;
                }
            } else if (count == 3) {
                new_value = 1;
            }

            tempArr[i][j] = new_value; // Store updated value in temporary matrix
        }
    }

    // Copy values from temporary matrix back to the original matrix
    #pragma omp parallel for collapse(2) schedule(dynamic, 3) if(ROWS > 20 && COLS > 20)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            arr[i][j] = tempArr[i][j];
        }
    }

    // Free memory used by the temporary matrix
    #pragma omp parallel for if(ROWS > 20)
    for (int i = 0; i < ROWS; i++) {
        free(tempArr[i]);
    }
    free(tempArr);
}

// main function
int main(int argc, char *argv[]){

    double elapsed_time, parallel_time, start_time, end_time, temp;
    int total_threads = atoi(argv[1]);

    omp_set_num_threads(total_threads);

    elapsed_time = -omp_get_wtime();
    // temp = -omp_get_wtime();
    int** board = createBoard();
    // temp += omp_get_wtime();
    // printf("%f", temp);

    srand(time(NULL));
    
    // Initializing the board
    parallel_time = -omp_get_wtime();
    initialize(board);
    parallel_time += omp_get_wtime();
    // printf("%f", parallel_time);

    // printBoard(board);
    
    // running for desired number of iterations
    for(int i = 0; i < 500; i++){
        // find next state
        start_time = omp_get_wtime();
        nextState(board);
        end_time = omp_get_wtime();
        parallel_time += (end_time - start_time);
        //printf("%f", parallel_time);


        // print new state
        // system("clear");
        // printBoard(board);
        // usleep(DELAY);
    }
    elapsed_time += omp_get_wtime();

    printf("\n%d", omp_get_max_threads());
    printf(",%f", elapsed_time);
    printf(",%f\n", parallel_time);
}