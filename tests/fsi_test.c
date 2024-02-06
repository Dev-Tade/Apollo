#define FSI_IMPLEMENTATION
#include "../fsi.h"

#include <string.h>

int main(int argc, char** argv) {
#if 0
  char *x = "Hello, world!\nthis is a test!\n";
  
  fsi_File first_test = fsi_FileFromCstr("fsi_test1.txt");
  size_t first_test_size = fsi_getFileSize(first_test);
  size_t first_test_written = fsi_writeFile(first_test, x, strlen(x));
  size_t first_test_read = 0;
  char * first_test_content = fsi_readFile(first_test, &first_test_read);

  printf("Test 1:\n\tsize=%zu\n\twritten=%zu\n\tread=%zu\n\tcontent=%s\n",
    first_test_size, first_test_written, first_test_read, first_test_content
  );

  struct __attribute__((packed)) {
    uint16_t fx;
    uint8_t vx;
  } annoyed[2] = {
    {.fx=0xEBCD,.vx=0xED},
    {.fx=0xFFCD,.vx=0xFD},
  };

  printf("%x\n", sizeof(annoyed));

  FILE *fh = fopen("fsi_test2.txt", "w+");
  fsi_File secnd_test = fsi_FileFromStdIO(fh);
  size_t secnd_test_written = fsi_writeFile(secnd_test, annoyed, sizeof(annoyed));
  size_t secnd_test_read = 0;
  char * secnd_test_content = fsi_readFile(secnd_test, &secnd_test_read);
  size_t secnd_test_size = fsi_getFileSize(secnd_test);

  printf("Test 2:\n\tsize=%zu\n\twritten=%zu\n\tread=%zu\n\tcontent=%s\n",
    secnd_test_size, secnd_test_written, secnd_test_read, secnd_test_content
  );
#endif

  fsi_File file = fsi_FileFromCstr("fsi_test2.txt");
  char* data = fsi_readFileEx(file, fsi_Offset(2, 5), NULL);
  for (size_t x=0; x<strlen(data); x++) {
    printf("%x ", (uint8_t)data[x]);
  }
  return 0;
}