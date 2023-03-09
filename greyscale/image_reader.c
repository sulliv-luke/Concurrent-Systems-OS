#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

typedef struct {
    unsigned char red, green, blue;
} Pixel;

int image_width = 0;
int image_height = 0;

Pixel** read_image(char* filename) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* infile;
    JSAMPARRAY buffer;
    int row_stride;
    Pixel** image = NULL;
    int i, j;

    printf(filename);
    // Open the input file
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s.\n", filename);
        exit(-1);
    }

    // Initialize the JPEG decompression object and error manager
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    // Read the JPEG header and decompress the image
    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);

    // Allocate memory for the image array
    image_width = cinfo.output_width;
    image_height = cinfo.output_height;
    image = (Pixel**) malloc(sizeof(Pixel*) * image_height);
    for (i = 0; i < image_height; i++) {
        image[i] = (Pixel*) malloc(sizeof(Pixel) * image_width);
    }

    // Read the image row-by-row and store the pixel values in the image array
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        for (i = 0, j = 0; i < row_stride; i += 3, j++) {
            image[cinfo.output_scanline-1][j].red = buffer[0][i];
            image[cinfo.output_scanline-1][j].green = buffer[0][i+1];
            image[cinfo.output_scanline-1][j].blue = buffer[0][i+2];
        }
    }

    // Finish decompressing the JPEG image and clean up
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return image;
}

void write_image(Pixel** image, int width, int height, char* filename) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* outfile;
    JSAMPROW row_pointer[1];
    int row_stride;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Can't open output file %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image[cinfo.next_scanline][0];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}

#include <stdio.h>
#include <jpeglib.h>

int get_jpeg_width(const char *filename) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile = fopen(filename, "rb");

    if (!infile) {
        fprintf(stderr, "Error opening JPEG file %s\n", filename);
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    
    jpeg_calc_output_dimensions(&cinfo);
    int width = cinfo.output_width;

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return width;
}


int get_jpeg_height(const char *filename) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile = fopen(filename, "rb");

    if (!infile) {
        fprintf(stderr, "Error opening JPEG file %s\n", filename);
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_calc_output_dimensions(&cinfo);
    int height = cinfo.output_height;

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return height;
}

void write_jpeg_image(char* filename, unsigned char** image, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE* outfile = fopen(filename, "wb");
    if (!outfile) {
        fprintf(stderr, "Error opening JPEG file %s\n", filename);
        return;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = width;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image[cinfo.next_scanline][0];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
}



