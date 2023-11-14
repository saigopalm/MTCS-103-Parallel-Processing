/* 
* Code to divide and send parts of an image to all the nodes in a cluster and then gather all the divded parts back and create the original image. 
* Author: Mantha Sai Gopal
* Reg.no: 23358
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv){
    
    int rank; // process id
    int size; // number of processes in the communicator
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *image_file;
    char *buffer_read, *buffer_write;
    long file_size;

    // Process 0 will read the image
    if(rank == 0){

        // Reading the image
        image_file = fopen("input.bmp", "rb");

        if(!image_file){
            printf("Unable to open the file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Get file size
        fseek(image_file, 0, SEEK_END);
        file_size = ftell(image_file);
        fseek(image_file, 0, SEEK_SET);

        // Allocating memory for the buffer to hold the image data
        buffer_read = (char*) malloc(file_size);

        if(!buffer_read){
            printf("Memory allocation failed.\n");
            fclose(image_file);
            return 1;
        }

        fread(buffer_read, 1, file_size, image_file);
        fclose(image_file);
    }

    // Broadcast the file_size to all processes
    MPI_Bcast(&file_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // Determining the size of message
    long message_size = file_size / size;

    // Allocating memory to store the recieved data
    buffer_write = (char *)malloc(message_size);

    // Scatter the message
    MPI_Scatter(buffer_read, message_size, MPI_CHAR, buffer_write, message_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    printf("\nScattered to process: %d\n", rank);
    
     // Now you have the scattered data in buffer_write, save it to a file
    char output_filename[20]; // Adjust the buffer size as needed
    snprintf(output_filename, 20, "output_%d.bmp", rank); // Create unique output filenames for each process

    FILE* output_file = fopen(output_filename, "wb");
    if (!output_file) {
        printf("Unable to open the output file for process %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    fwrite(buffer_write, 1, message_size, output_file);
    fclose(output_file);

    // Gather the data back to process 0
    MPI_Gather(buffer_write, message_size, MPI_CHAR, buffer_read, message_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        FILE* output_file = fopen("gathered_image.bmp", "wb");
        if (output_file) {
            fwrite(buffer_read, 1, file_size, output_file);
            fclose(output_file);
        } else {
            printf("Unable to open the output file.\n");
        }

        free(buffer_read);
    }

    free(buffer_write);
    
    MPI_Finalize();
    return 0;
}
