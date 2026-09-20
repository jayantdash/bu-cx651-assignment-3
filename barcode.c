#include "barcode.h"
#include <stdio.h>
#include "common.h"


struct image* barcode(char* data, int width, int height) {
    struct image* img = malloc(sizeof(struct image));
    if (img == NULL) {
        return NULL;
    }

    img->width = width;
    img->height = height;
    
    img->pixels = malloc(sizeof(struct pixel) * width * height);
    if (img->pixels == NULL) {
        printf("Failed to allocate memory for image pixels.\n");
        // Free the previously allocated image structure before returning.
        free(img);
        return NULL;
    }

    return img;
}

