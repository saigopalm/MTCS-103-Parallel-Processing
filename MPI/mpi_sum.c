/*
* MPI program to sum 100 elements.
* Author: Mantha Sai Gopal
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int n = 100; // The number of elements to sum
    int local_sum = 0;
    int total_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate the local sum for each process
    for (int i = rank; i < n; i += size) {
        local_sum += i;
    }

    // Reduce the local sums to compute the total sum
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("The total sum is: %d\n", total_sum);
    }

    MPI_Finalize();
    return 0;
}
