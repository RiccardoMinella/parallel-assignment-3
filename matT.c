#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv) {
    // Initialize MPI
    int MPI_rank, MPI_size;
    int matrix_size, strip_size;
    double wt1, wt2;
    float *Adata, *transposed_Adata;
    float *stripdata, *transposed_stripdata;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);

    // Handle arguments
    if (argc != 2) {
        if (MPI_rank == 0)
            fprintf(stderr, "Error - not enough arguments\n"
                            "Usage: %s <Size>\n", argv[0]);

        MPI_Finalize();
        return -1;
    }

    if (sscanf(argv[1], "%i", &matrix_size) != 1) {
        if (MPI_rank == 0)
            fprintf(stderr, "Error - <Row size> not an integer\n"
                            "Usage: %s <Size> <Block size>\n", argv[0]);

        MPI_Finalize();
        return -1;
    }

    // Check compatibility between number of processors and matrix size
    if (matrix_size % MPI_size != 0) {
        if (MPI_rank == 0)
            printf("Error - Incompatible number of MPI processes and matrix dimensions.\n");

        MPI_Finalize();
        return 1;
    }   

    /* calculate the strip size */
    strip_size = matrix_size / MPI_size;

    if (MPI_rank == 0)
    {
        /* genarate Matrix A */
        Adata = (float *)malloc(sizeof(float) * matrix_size * matrix_size);
        transposed_Adata = (float *)malloc(sizeof(float) * matrix_size * matrix_size);
        
        for (int i = 0; i < matrix_size; i++)
            for (int j = 0; j < matrix_size; j++)
                Adata[i * matrix_size + j] = i * matrix_size + j;
    }

    stripdata = (float *)malloc(sizeof(float) * strip_size * matrix_size);
    transposed_stripdata = (float *)malloc(sizeof(float) * strip_size * matrix_size);

    MPI_Scatter(Adata, matrix_size*strip_size, MPI_FLOAT, stripdata, matrix_size*strip_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    wt1 = MPI_Wtime();

    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < strip_size; j++)
        {
            transposed_stripdata[i * strip_size + j] = stripdata[i + j * matrix_size];
        }
    }

    for (int i = 0; i < matrix_size; i++)
        MPI_Gather(&(transposed_stripdata[i*strip_size]), strip_size, MPI_FLOAT, &(transposed_Adata[i*matrix_size]), strip_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (MPI_rank == 0)
    {
        wt2 = MPI_Wtime();
        printf("%f\n", wt2 - wt1);
    }

    // Clean up and exit
    if (MPI_rank == 0)
    {
        free(Adata);
        free(transposed_Adata);
    }
    free(transposed_stripdata);
    free(stripdata);

    MPI_Finalize();

    return 0;
}
