/**
 * Program for calculating PI using MPI
 * Author: Mantha Sai Gopal
 * Reg.no: 23358
*/

#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	int my_rank, size, n, i;
	double sum, h, x, my_pi, pi;

	MPI_Init(&argc, &argv); // Initializing MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if(my_rank == 0){
		printf("Enter the number of intervals: \n");
		scanf("%d",&n);
	}	

    // Broadcasting the value of pi to all the processes in the communicator
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
    sum = 0.0;
	h = 1.0 / (double)n;
	for (i = my_rank + 1; i <= n; i += size){
		x = h * ((double) i - 0.5);
		sum += 4.0 / (1.0 + x * x);
	}
	
    my_pi = h * sum;
	MPI_Reduce(&my_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0){
		printf("PI is aproximately %20.16f\n", pi);
	}

	MPI_Finalize();
	return 0;
}
