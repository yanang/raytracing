#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    struct timespec start, end;

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");

    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    para ptr[4];
    pthread_t pid0, pid1, pid2, pid3;

    ptr[0].pixels = pixels;
    ptr[0].rectangulars = rectangulars;
    ptr[0].spheres = spheres;
    ptr[0].lights = lights;
    ptr[0].view = &view;
    ptr[0].width_start = 0;
    ptr[0].width = 256;
    ptr[0].height_start = 0;
    ptr[0].height = 256;
    pthread_create(&pid0, NULL, raytracing, (void *)&ptr[0]);

    ptr[1].pixels = pixels;
    ptr[1].rectangulars = rectangulars;
    ptr[1].spheres = spheres;
    ptr[1].lights = lights;
    ptr[1].view = &view;
    ptr[1].width_start = 0;
    ptr[1].width = 256;
    ptr[1].height_start = 256;
    ptr[1].height = 512;
    pthread_create(&pid1, NULL, raytracing, (void *)&ptr[1]);

    ptr[2].pixels = pixels;
    ptr[2].rectangulars = rectangulars;
    ptr[2].spheres = spheres;
    ptr[2].lights = lights;
    ptr[2].view = &view;
    ptr[2].width_start = 256;
    ptr[2].width = 512;
    ptr[2].height_start = 256;
    ptr[2].height = 512;
    pthread_create(&pid2, NULL, raytracing, (void *)&ptr[2]);

    ptr[3].pixels = pixels;
    ptr[3].rectangulars = rectangulars;
    ptr[3].spheres = spheres;
    ptr[3].lights = lights;
    ptr[3].view = &view;
    ptr[3].width_start = 256;
    ptr[3].width = 512;
    ptr[3].height_start = 0;
    ptr[3].height = 256;
    pthread_create(&pid3, NULL, raytracing, (void *)&ptr[3]);


    void *ret;
    pthread_join(pid0,&ret);
    pthread_join(pid1,&ret);
    pthread_join(pid2,&ret);
    pthread_join(pid3,&ret);

    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    return 0;
}
