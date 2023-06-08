#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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
    double forces[NUM_OBJECTS][2];
    int i, j;

    // Initialize forces array
    for (i = 0; i < NUM_OBJECTS; i++) {
        forces[i][0] = 0.0;
        forces[i][1] = 0.0;
    }

    // Compute forces between objects
    for (i = 0; i < NUM_OBJECTS; i++) {
        for (j = 0; j < NUM_OBJECTS; j++) {
            if (i != j) {
                double dx = objects[j].x - objects[i].x;
                double dy = objects[j].y - objects[i].y;
                double distance = sqrt(dx*dx + dy*dy);
                double force_magnitude = (GRAVITATIONAL_CONSTANT * objects[i].mass * objects[j].mass) / (distance * distance);
                double force_x = force_magnitude * (dx / distance);
                double force_y = force_magnitude * (dy / distance);
                forces[i][0] += force_x;
                forces[i][1] += force_y;
            }
        }
    }

    // Update velocities based on forces
    for (i = 0; i < NUM_OBJECTS; i++) {
        objects[i].velocity_x += forces[i][0] * TIME_STEP;
        objects[i].velocity_y += forces[i][1] * TIME_STEP;
    }

    // Update positions based on velocities
    for (i = 0; i < NUM_OBJECTS; i++) {
        objects[i].x += objects[i].velocity_x * TIME_STEP;
        objects[i].y += objects[i].velocity_y * TIME_STEP;
    }
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

    // Simulate the system for a number of iterations
    #pragma omp parallel for
    for (iteration = 1; iteration <= num_iterations; iteration++) {
        simulate(objects);

        // Print positions after each iteration
        printf("Iteration %d:\n", iteration);
        for (int i = 0; i < NUM_OBJECTS; i++) {
            printf("Object %d: x=%.2f, y=%.2f\n", i+1, objects[i].x, objects[i].y);
        }
        printf("\n");
    }

    return 0;
}
