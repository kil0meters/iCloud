#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <png.h>

#define RETURN_IF_NULL(val, err_msg) if (val == NULL) { fputs(err_msg "\n", stderr); return false; }

typedef struct Dimentions {
    int width;
    int height;
    int dead_pixels;
} Dimentions;

typedef struct ImageData {
    uint8_t *data;
    uint32_t num_bytes;
} ImageData;

static ImageData read_file_into_data(const char *filename) {
    FILE *f = fopen(filename, "rb");
    uint8_t *data = NULL;
    long fsize = 0;
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        data = malloc(fsize);
        fread(data, fsize, sizeof(uint8_t), f);
        fclose(f);
    }

    ImageData image_data = {
        .data = data,
        .num_bytes = fsize
    };
    return image_data;
}

// find image dimentions from the number of bytes
static Dimentions find_image_dimentions(uint32_t num_bytes) {
    Dimentions dimentions = {0, 0, 256};

    uint32_t length = num_bytes / 3;
    double length_sqrt = sqrt(length);
    uint32_t length_floor = floor(length_sqrt);
    uint32_t length_ceil = ceil(length_sqrt);

    // find the most sqaure image possible
    if (length_floor == length_ceil) {
        dimentions.width = length_floor;
        dimentions.height = length_ceil;
        dimentions.dead_pixels = 0;
    } else if (length_ceil * length_floor > length) {
        dimentions.width = length_floor;
        dimentions.height = length_ceil;
        dimentions.dead_pixels = (length_ceil * length_floor) - length;
    } else {
        dimentions.width = length_ceil;
        dimentions.height = length_ceil;
        dimentions.dead_pixels = (length_ceil * length_ceil) - length;
    }

    // add on byte for rounding
    if (num_bytes % 3 > 0)
        dimentions.dead_pixels += 1;

    return dimentions;
}

static png_structp get_png_struct(const char *filename, uint32_t width, uint32_t height) {
    FILE *file_to_write = fopen(filename, "wb");
    RETURN_IF_NULL(file_to_write, "error: Could not open output file");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    RETURN_IF_NULL(png_ptr, "error: Not enough memory");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    RETURN_IF_NULL(info_ptr, "error: Not enough memory");

    png_init_io(png_ptr, file_to_write);

	png_init_io(png_ptr, file_to_write);

	png_set_IHDR(png_ptr, info_ptr, width,
				 height, 8, PNG_COLOR_TYPE_RGB,
				 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
				 PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	return png_ptr;
}

// void write_header(png_structp, )

bool encode_png(
    const char *filename,
    const char *dest
) {
    printf("encoding file \"%s\" to \"%s\"\n", filename, dest);

    ImageData image_data = read_file_into_data(filename);
    RETURN_IF_NULL(image_data.data, "error: Could not open input file");

    // the first byte of the png indicates how many dead pixels there are
    // the second byte is the file part
    // the 255 bytes after that are the name of the file
    uint32_t content_length = image_data.num_bytes + 1 + 1 + 255;

    Dimentions image_dimentions = find_image_dimentions(content_length);
    printf("size: %d\nlength: %d\nwidth: %d\n\n", content_length,
           image_dimentions.width, image_dimentions.height);

	png_structp png_ptr = get_png_struct(dest,
                                         image_dimentions.width,
                                         image_dimentions.height);

    uint32_t bytes_per_row = 3 * image_dimentions.width;
    png_bytep row = calloc(sizeof(png_byte), bytes_per_row);

    // char *new_filename = calloc(sizeof(char), 255);


    size_t rows_written = 0;
    size_t data_index = 0;
    // write header and other necessary other bytes
    if (bytes_per_row > 256) {
        row[0] = image_dimentions.dead_pixels;
        row[1] = 0;

        for (size_t i = 0; i < strlen(filename); i++)
            row[i + 2] = filename[i];

        for (size_t x = 256; x < bytes_per_row; x++) {
            row[x] = image_data.data[data_index];
            data_index++;
        }

        png_write_row(png_ptr, row);
        rows_written = 1;
    }
    else {
        RETURN_IF_NULL(NULL, "ERROR: FILE TOO SMALL");
        // row[0] = image.dimentions.dead_pixels;
        // row[1] = 0;
    }

    // don't override header information
    for (size_t y = rows_written; y < image_dimentions.height; y++) {
        if (y == image_dimentions.height - 1) {
            for (size_t x = bytes_per_row - image_dimentions.dead_pixels; x < bytes_per_row; x++) {
                row[x] = 0;
            }
            bytes_per_row -= image_dimentions.dead_pixels;
        }
        for (size_t x = 0; x < bytes_per_row; x++) {
            row[x] = image_data.data[data_index];
            data_index++;
        }

        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, NULL);

    return true;
}
