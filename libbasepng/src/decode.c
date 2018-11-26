#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <png.h>

#define EXIT_IF(expression, err_msg) if (expression) { \
    fputs("error: " err_msg "\n", stderr); \
	exit(1); \
}

typedef struct Image {
    png_structp png_ptr;
    png_infop info_ptr;
    uint32_t width;
    uint32_t height;
} Image;

typedef struct Header {
    uint8_t dead_pixels;
    uint8_t file_part;
    const char *filename;
} Header;

static Image png_read_from_file(const char *filename) {
    uint8_t header[8];
    FILE *fp = fopen(filename, "rb");
    EXIT_IF(fp == NULL, "Could not open input file");

    EXIT_IF(fread(header, sizeof(uint8_t), 8, fp) != 8, "Encountered problem when reading file");
    EXIT_IF(png_sig_cmp(header, 0, 8), "Invalid PNG file");

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    EXIT_IF(png_ptr == NULL, "Not enough memory");

    png_init_io(png_ptr, fp);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    EXIT_IF(info_ptr == NULL, "Not enough memory");

    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    uint32_t width = png_get_image_width(png_ptr, info_ptr);
    uint32_t height = png_get_image_height(png_ptr, info_ptr);

    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    EXIT_IF(bit_depth != 8, "Invalid BasePNG file");

    Image image = {
        .png_ptr = png_ptr,
        .info_ptr = info_ptr,
        .width = width,
        .height = height
    };

    return image;
}

static Header basepng_read_header(png_bytep row) {
    uint8_t dead_pixels = row[0];
    uint8_t part_index = row[1];

    static char filename[255];
    int i = 0;
    while (row[2] != '\0') {
        filename[i] = row[2];
        i++;
        row++;
    }

    Header header = {
        .dead_pixels = dead_pixels,
        .file_part = part_index,
        .filename = filename
    };

    return header;
}

static bool file_exists(const char *filename) {
    struct stat st;
    int result = stat(filename, &st);
    return result == 0;
}

bool decode_png(const char *input) {
    Image image = png_read_from_file(input);

    png_bytep *row_pointers = calloc(sizeof(png_bytep*), image.height);
    for (int y = 0; y < image.height; y++) {
        row_pointers[y] = (png_byte*)
        malloc(png_get_rowbytes(image.png_ptr,image.info_ptr));
    }

    png_read_image(image.png_ptr, row_pointers);

    Header header = basepng_read_header(row_pointers[0]);

	printf("filename: %s\nlength: %d\nheight: %d\nnumber_of_bytes: %d\ndead_pixels: %d\nfile_part: %d\n",
			header.filename, image.width, image.height,
            image.width * 3 * image.height - header.dead_pixels,
            header.dead_pixels, header.file_part);

    uint32_t bytes_per_row = image.width * 3;

    EXIT_IF(file_exists(header.filename), "Output file already exists");
    FILE *output = fopen(header.filename, "ab");

	fwrite(row_pointers[0] + 256, bytes_per_row - 256, sizeof(uint8_t), output);
    for (size_t y = 1; y < image.height; y++) {
        if (y == image.height - 1)
            bytes_per_row -= header.dead_pixels;
		fwrite(row_pointers[y], bytes_per_row, sizeof(uint8_t), output);
	}

	fclose(output);
    return true;
}
