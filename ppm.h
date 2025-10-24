#ifndef PPM_H_
#define PPM_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  unsigned char r, g, b;
} Pixel;

typedef enum {
  PPM_P3,
  PPM_P6,
} Type;

typedef struct {
  Type type;
  unsigned int width, height, max_value;
  Pixel **pixels;
} Image;


Image *read_image(const char *filepath);
void destroy(Image *image);
bool write_image(const char *filepath, Image *image);
const char *strtype(Type type);


#ifdef __cplusplus
}
#endif

#endif // PPM_H


#ifdef PPM_IMPLEMENTATION
  Image *read_image(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
      return NULL;
    }
    Image *image = (Image *)malloc(sizeof(Image));

    // read header
    // magic num
    char magic_num[3];
    char line[256];
    fgets(line, sizeof(line), file);
    sscanf(line, "%s", magic_num);
    if (strcmp(magic_num, "P6") != 0) {
      free(image);
      fclose(file);
      return NULL;
    }
    image->type = PPM_P6;

    // width height
    while (fgets(line, sizeof(line), file)) {
      if (line[0] == '#') continue;
      sscanf(line, "%u %u", &image->width, &image->height);
      break;
    }

    // max value (color)
    while (fgets(line, sizeof(line), file)) {
      if (line[0] == '#') continue;
      sscanf(line, "%u", &image->max_value);
      break;
    }
    // fgetc(file);

    // allocate resource
    image->pixels = (Pixel **)malloc(image->height * sizeof(Pixel *));
    if (!image->pixels) {
      free(image);
      fclose(file);
      return NULL;
    }

    for (int i = 0; i < image->height; i++) {
      image->pixels[i] = (Pixel*)malloc(image->width * sizeof(Pixel));
      if (!image->pixels[i]) {
        for (int j = 0; j < i; j++) free(image->pixels[j]);
        free(image->pixels);
        free(image);
        fclose(file);
        return NULL;
      }
    }

    // read pixel by pixel
    for (int i = 0; i < image->height; i++) {
      size_t element = fread(image->pixels[i], sizeof(Pixel), image->width, file);
      if (element != image->width) {
        destroy(image);
        fclose(file);
        return NULL;
      }
    }
    
    fclose(file);
    return image;
  }

  void destroy(Image *image) {
    if (!image) return;
    for (int i = 0; i < image->height; i++) {
      free(image->pixels[i]);
    }
    free(image->pixels);
    free(image);
  }

  bool write_image(const char *filepath, Image *image) {
    if (
      image->width <= 0 || image->height <= 0 || 
      image->max_value != 255 || image->type != PPM_P6 ||
      image == NULL
    ) {
      fprintf(stderr, "[ERROR] Invalid image\n");
      return false;
    }

    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
      fprintf(stderr, "[ERROR] Failed to write image\n");
      return false;
    }

    if (fprintf(file, "P6\n%u %u\n%u\n", image->width, image->height, image->max_value) < 0) {
      fprintf(stderr, "[ERROR] Failed to write header\n");
      fclose(file);
      return false;
    }
    
    size_t pixel_size = image->width * image->height;
    Pixel *buffer = (Pixel *)malloc(pixel_size * sizeof(Pixel));
    if (buffer == NULL) {
      fprintf(stderr, "[ERROR] Failed to allocate buffer\n");
      fclose(file);
      return false;
    }

    for (unsigned int i = 0; i < image->height; i++) {
      for (unsigned int j = 0; j < image->width; j++) {
        buffer[i * image->width + j] = image->pixels[i][j];
      }
    }
    
    size_t written_pixels = fwrite(buffer, sizeof(Pixel), pixel_size, file);
    free(buffer);
    if (written_pixels!= pixel_size) {
      fprintf(stderr, "[ERROR] Failed to write pixels\n");
      fclose(file);
      return false;
    }

    fclose(file);
    return true;
  }

  const char *strtype(Type type) {
    switch (type) {
    case PPM_P3:
    return "P3";
    case PPM_P6:
    return "P6";
    default:
    return "Invalid Type";
    }
  }

#endif
