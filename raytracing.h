#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

void *raytracing(void *paraptr);
typedef struct __PARAMETER {
    uint8_t *pixels;
    rectangular_node rectangulars;
    sphere_node spheres;
    light_node lights;
    const viewpoint *view;
    int width_start;
    int width;
    int height_start;
    int height;
} para;
#endif
