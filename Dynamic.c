#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

// Function to calculate the Mandelbrot set
int mandelbrot(double real, double imag) {
    double r = real;
    double i = imag;
    int n;
    for (n = 0; n < MAX_ITER; n++) {
        double r2 = r * r;
        double i2 = i * i;
        if (r2 + i2 > 4.0) return n;
        i = 2 * r * i + imag;
        r = r2 - i2 + real;
    }
    return MAX_ITER;
}

// Function to generate the Mandelbrot image for a specific part
void generate_mandelbrot_part(uint8_t* buffer, int start_row, int end_row) {
    for (int y = start_row; y < end_row; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            int value = mandelbrot(real, imag);

            // Map the value to grayscale (0-255)
            uint8_t gray_value = (uint8_t)((value % 256) * 255.0 / 255);

            buffer[y * WIDTH + x] = gray_value;
        }
    }
}

// Function to save the image as a PGM file
void save_pgm(const char* filename, uint8_t* buffer) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fprintf(file, "P5\n%d %d\n255\n", WIDTH, HEIGHT);
    fwrite(buffer, sizeof(uint8_t), WIDTH * HEIGHT, file);

    fclose(file);
}

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = HEIGHT / size;
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;

    uint8_t* local_buffer = (uint8_t*)malloc(WIDTH * rows_per_process * sizeof(uint8_t));

    clock_t start_time = clock();

    generate_mandelbrot_part(local_buffer, start_row, end_row);

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Process %d: Execution Time: %.2f seconds\n", rank, execution_time);

    uint8_t* global_buffer = NULL;
    if (rank == 0) {
        global_buffer = (uint8_t*)malloc(WIDTH * HEIGHT * sizeof(uint8_t));
    }

    MPI_Gather(local_buffer, WIDTH * rows_per_process, MPI_UNSIGNED_CHAR,
               global_buffer, WIDTH * rows_per_process, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        save_pgm("mandelbrot.pgm", global_buffer);
        free(global_buffer);
    }

    free(local_buffer);

    MPI_Finalize();

    return 0;
}
