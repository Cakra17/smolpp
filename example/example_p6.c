#include <errno.h>
#define PPM_IMPLEMENTATION
#include "../ppm.h"

int main(int argc, char **argv) {
  Image *image = read_image("./example/sample_640×426.ppm");
  if (image == NULL) {
    printf("[ERROR] %s", strerror(errno));
    return 1;
  }

  printf("---HEADER----\n");
  printf("Type : %s\n", strtype(image->type));
  printf("width : %d\n", image->width);
  printf("height : %d\n", image->height);
  printf("max_val (colors) : %d\n", image->max_value);

  printf("SAVE FILE AS new smolpp\n");
  if (!write_image("./smolpp.ppm", image)) {
    printf("[ERROR] %s", strerror(errno));
    return 1;
  }

  if (image != NULL) destroy(image);
  return 0;
}
