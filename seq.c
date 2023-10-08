#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

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

int main() {
    uint8_t* buffer = (uint8_t*)malloc(WIDTH * HEIGHT * sizeof(uint8_t));

    clock_t start_time = clock();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double real = (x - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (y - HEIGHT / 2.0) * 4.0 / HEIGHT;
            int value = mandelbrot(real, imag);

            // Map the value to grayscale (0-255)
            uint8_t gray_value = (uint8_t)((value % 256) * 255.0 / 255);

            buffer[y * WIDTH + x] = gray_value;
        }
    }

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Execution Time: %.2f seconds\n", execution_time);

    // You can save the image or perform other operations here

    free(buffer);
    return 0;
}