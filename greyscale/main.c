#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <jpeglib.h>
#include "image_reader.c"

const int NUM_THREADS = 10;

Pixel ** image;
Pixel ** greyImage;
int image_height;
int image_width;

void* thread_function(void* arg) {
    // Get the arguments passed to the thread by extracting value that is pointed to by arg, and casting it from a void pointer to an int pointer
    int thread_num = *((int*) arg); 
    // Calculate the number of rows this thread needs to loop over based on the height of the image
    int rows_per_thread = image_height / NUM_THREADS;
    // Calculate the rows this thread will start and end at
    int start_row = thread_num * rows_per_thread;
    int end_row = (thread_num == NUM_THREADS - 1) ? image_height - 1 : (thread_num + 1) * rows_per_thread - 1;

    // Convert the pixels in the portion of the image assigned to this thread to grey-scale
    for (int i = start_row; i <= end_row; i++) {
        for (int j = 0; j < image_width; j++) {
            // Calculate the grey-scale value of the pixel
            int r = image[i][j].red;
            int g = image[i][j].green;
            int b = image[i][j].blue;
            int grey = (r + g + b) / 3;

            // Store the grey-scale value in the output image
            greyImage[i][j].red = grey;
            greyImage[i][j].green = grey;
            greyImage[i][j].blue = grey;
        }
    }

    // Exit the thread
    pthread_exit(NULL);
}

void create_threads() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i, rc;

    // Create ten threads, each running the thread_function() and passing its thread number as an argument
    for (i = 0; i < 10; i++) {
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d. Return code: %d\n", i, rc);
            exit(-1);
        }
    }

    // Wait for all threads to complete
    for (i = 0; i < 10; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc) {
            fprintf(stderr, "Error joining thread %d. Return code: %d\n", i, rc);
            exit(-1);
        }
    }

    // All threads have completed
    printf("All threads completed successfully.\n");
}

int main (int argc, const char * argv[]) {
  int min = 2;
  if (argc < min) {
        printf("Usage: %s <input_image_filename>\n", argv[0]);
        return -1;
   }

  printf("start");

  image_height = get_jpeg_height(argv[min-1]);
  image_width = get_jpeg_width(argv[min-1]);

    // Read in the input image
    image = read_image(argv[min-1]);
    greyImage = (Pixel**) malloc(sizeof(Pixel*) * image_height);
    for (int i = 0; i < image_height; i++) {
        greyImage[i] = (Pixel*) malloc(sizeof(Pixel) * image_width);
    }

    // Process the image using ten threads
    create_threads();

    // Output new grey image
    write_image(greyImage, image_width, image_height, "new.jpg");

    printf("Done");

    return 0;

  
}
