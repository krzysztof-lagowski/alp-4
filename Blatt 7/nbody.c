#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_OBJECTS 3
#define GRAVITATIONAL_CONSTANT 6.67430e-11
#define TIME_STEP 0.1

typedef struct {
    double x;
    double y;
    double mass;
    double velocity_x;
    double velocity_y;
} Object;

void simulate(Object objects[]) {
    // ... your simulate() function code ...
}

int main() {
    Object objects[NUM_OBJECTS];

    int num_iterations = 10;
    int iteration;

    // Seed the random number generator
    srand(time(NULL));

    // Initialize random positions and velocities for the objects
    for (int i = 0; i < NUM_OBJECTS; i++) {
        objects[i].x = ((double)rand() / RAND_MAX) * 20 - 10;  // Range: -10 to 10
        objects[i].y = ((double)rand() / RAND_MAX) * 20 - 10;  // Range: -10 to 10
        objects[i].mass = 50.0;
        objects[i].velocity_x = (double)rand() / RAND_MAX;
        objects[i].velocity_y = (double)rand() / RAND_MAX;
    }

    // Print initial positions
    for (int i = 0; i < NUM_OBJECTS; i++) {
        printf("Object %d: x=%.2f, y=%.2f\n", i+1, objects[i].x, objects[i].y);
    }
    printf("\n");

    // Time measurement variables
    clock_t start_time, end_time;
    double execution_time;

    // Start the timer
    start_time = clock();

    // Simulate the system for a number of iterations
    for (iteration = 1; iteration <= num_iterations; iteration++) {
        simulate(objects);

        // Print positions after each iteration
        printf("Iteration %d:\n", iteration);
        for (int i = 0; i < NUM_OBJECTS; i++) {
            printf("Object %d: x=%.2f, y=%.2f\n", i+1, objects[i].x, objects[i].y);
        }
        printf("\n");
    }

    // Stop the timer
    end_time = clock();

    // Calculate the execution time
    execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Print the execution time
    printf("Execution time: %.2f seconds\n", execution_time);

    return 0;
}
