#include "barcode.h"
#include <stdio.h>
#include "common.h"
#include <stdbool.h>
#include <string.h>

#define BARCODE_LENGTH 12
#define BARCODE_WIDTH 113
#define QUIET_ZONE_WIDTH 9
#define START_END_WIDTH 3
#define MIDDLE_WIDTH 5

#define START_PATTERN 0b010
#define MIDDLE_PATTERN 0b10101
#define END_PATTERN 0b010

#define DIGIT_WIDTH 7

static const int LEFT_DIGITS[10] = {
    0b1110010,  // 0
    0b1100110,  // 1
    0b1101100,  // 2
    0b1010000,  // 3
    0b1011100,  // 4
    0b1001110,  // 5
    0b1000010,  // 6
    0b1000100,  // 7
    0b1001000,  // 8
    0b1110100   // 9
};

static int GET_BIT(int value, int bit, int index) {
    return (value >> (bit - 1 - index)) & 1;
}

static void fill_bar(struct image *img, int start_pos, int value) {
    for (int h = 0; h < img->height; h++) {
        int index = h * img->width + start_pos;

        // Determine the color of the bar based on the value (1 = white, 0 = black)
        if (value == 1) {
            img->pixels[index].r = 255;
            img->pixels[index].g = 255;
            img->pixels[index].b = 255;
        } else {
            img->pixels[index].r = 0;
            img->pixels[index].g = 0;
            img->pixels[index].b = 0;
        }
    }
}

static void fill_pattern(struct image *img, int start_pos, int pattern, int width) {
    for (int w = 0; w < width; w++) {
        int value = GET_BIT(pattern, width, w);
        fill_bar(img, start_pos + w, value);
    }
}

static void fill_digit(struct image *img, int start_pos, int digit, bool left_side) {
    int code = LEFT_DIGITS[digit];
    for (int w = 0; w < DIGIT_WIDTH; w++) {
        int value = GET_BIT(code, DIGIT_WIDTH, w);
        // Invert the value for right-side digits, else keep it as is.
        if (!left_side) {
            value = !value;
        }
        fill_bar(img, start_pos + w, value);
    }
}

static int get_char_to_digit(char c) {
    return c - '0';
}

struct image* barcode(char* data, int width, int height) {

    // Validate input.
    if (data == NULL) {
        return NULL;
    }

    // Check if the input data has the correct length.
    if (strlen(data) != BARCODE_LENGTH) {
        return NULL;
    }

    // Check if the image dimensions are valid.
    if (width < BARCODE_WIDTH || height <= 0) {
        return NULL;
    }

    // Check if the input data contains only digits.
    for (int i = 0; i < BARCODE_LENGTH; i++) {
        if (data[i] < '0' || data[i] > '9') {
            return NULL;
        }
    }

    // Allocate and initialize the image structure.
    struct image* img = malloc(sizeof(struct image));
    if (img == NULL) {
        return NULL;
    }

    img->width = width;
    img->height = height;

    img->pixels = malloc(sizeof(struct pixel) * width * height);
    if (img->pixels == NULL) {
        // Free the previously allocated image structure before returning.
        free(img);
        return NULL;
    }

    int start_pos = 0;

    for (int i = 0; i < QUIET_ZONE_WIDTH; i++) {
        fill_bar(img, start_pos++, 1);
    }

    fill_pattern(img, start_pos, START_PATTERN, START_END_WIDTH);
    start_pos += START_END_WIDTH;

    // Fill the left-side digits.
    for (int i = 0; i < 6; i++) {
        int digit = get_char_to_digit(data[i]);
        fill_digit(img, start_pos, digit, true);
        start_pos += DIGIT_WIDTH;
    }

    // Fill the middle pattern.
    fill_pattern(img, start_pos, MIDDLE_PATTERN, MIDDLE_WIDTH);
    start_pos += MIDDLE_WIDTH;

    // Fill the right-side digits.
    for (int i = 6; i < 12; i++) {
        int digit = get_char_to_digit(data[i]);
        fill_digit(img, start_pos, digit, false);
        start_pos += DIGIT_WIDTH;
    }

    // Fill the end pattern.
    fill_pattern(img, start_pos, END_PATTERN, START_END_WIDTH);
    start_pos += START_END_WIDTH;

    // Fill the trailing quiet zone.
    for (int i = 0; i < QUIET_ZONE_WIDTH; i++) {
        fill_bar(img, start_pos++, 1);
    }

    return img;
}