#ifndef FSI_H
#define FSI_H

#ifndef APOLLO_DEF
#define APOLLO_DEF
#else
#undef APOLLO_DEF
#define APOLLO_DEF
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/*
  Struct abstracting files, when not using stdio _iobuf automatically closes files
  @param filePath: file path as char*
  @param fileHandle: file handle as _iobuf (stdio.h)
  @param tag: set to 1 if using _iobuf (FILE*)
*/
typedef struct {
  union {
    char *filePath;
    FILE *fileHandle;
  };
  uint8_t tag;
} fsi_File;

/*
  Struct containign begin and end offsets for file operations
  @param begin: beginning of operation
  @param end: end of operation
*/
typedef struct {
  size_t begin;
  size_t end;
} fsi_Offset;

#define fsi_Offset(_begin, _end) ((fsi_Offset){.begin=_begin, .end=_end})

/*
  Creates a fsi_File abstraction from a string path
  @param path: path to file
  @returns a fsi_File Struct with necesary information
*/
APOLLO_DEF fsi_File fsi_FileFromCstr(char *path);

/*
  Creates a fsi_File abstraction from a stdio::FILE*
  @param iobuf: FILE* iobuf
  @returns a fsi_File Struct with necesary information
*/
APOLLO_DEF fsi_File fsi_FileFromStdIO(FILE *iobuf);

/*
  Gets size of specified file
  @param file: handle containing path to file
  @returns the size of the file in bytes
*/
APOLLO_DEF size_t fsi_getFileSize(fsi_File file);

/*
  Read all contents from a file
  @param file: handle containing path to file
  @param bytesRead: gets modified with the value of readed bytes, unless NULL
  @return a malloc'd memory block containing file contents
*/
APOLLO_DEF char *fsi_readFile(fsi_File file, size_t *bytesRead);

/*
  Writes contents to file (destructive)
  @param file: handle containing path to file
  @param content: pointer to content
  @param n: sizeof content
  @return number of bytes written
*/
APOLLO_DEF size_t fsi_writeFile(fsi_File file, void *content, size_t n);

/*
  Read contents from a file
  @param file: handle containing path to file
  @param offset: offset specifying range of read operation
  @param bytesRead: gets modified with the value of readed bytes, unless NULL
  @return a malloc'd memory block containing file contents specified in offset range
*/
APOLLO_DEF char *fsi_readFileEx(fsi_File file, fsi_Offset offset, size_t *bytesRead);

#endif

#ifdef FSI_IMPLEMENTATION

#ifndef APOLLO_DEF
#define APOLLO_DEF static
#else
#undef APOLLO_DEF
#define APOLLO_DEF static
#endif

#include <stdlib.h>

APOLLO_DEF inline fsi_File fsi_FileFromCstr(char *path) {
  return (fsi_File){.filePath=path, .tag=0};
}

APOLLO_DEF inline fsi_File fsi_FileFromStdIO(FILE *iobuf) {
  return (fsi_File){.fileHandle=iobuf, .tag=1};
}

APOLLO_DEF size_t fsi_getFileSize(fsi_File file) {
  size_t ret = 0;
  if (!(file.tag)) {
    FILE *tmp = fopen(file.filePath, "r");
    fseek(tmp, 0, SEEK_END);
    ret = ftell(tmp);
    fseek(tmp, 0, SEEK_SET);
    fclose(tmp);
  } else {
    fseek(file.fileHandle, 0, SEEK_END);
    ret = ftell(file.fileHandle);
    fseek(file.fileHandle, 0, SEEK_SET);
  }

  return ret;
}

APOLLO_DEF char *fsi_readFile(fsi_File file, size_t *bytesRead) {
  size_t size = fsi_getFileSize(file);
  char *ret = (char*)malloc(size+1);
  size_t read = 0;

  if (!(file.tag)) {
    FILE* tmp = fopen(file.filePath, "rb");
    read = fread(ret, 1, size, tmp);
    fclose(tmp);
  } else {
    read = fread(ret, 1, size, file.fileHandle);
  }

  if (bytesRead != NULL) *bytesRead = read;
  ret[size] = '\0'; 
  return ret;
}

APOLLO_DEF size_t fsi_writeFile(fsi_File file, void *content, size_t n) {
  size_t ret = 0;

  if (!(file.tag)) {
    FILE* tmp = fopen(file.filePath, "wb");
    ret = fwrite(content, 1, n, tmp);
    fclose(tmp);
  } else {
    ret = fwrite(content, 1, n, file.fileHandle);
  }

  return ret;
}

APOLLO_DEF char *fsi_readFileEx(fsi_File file, fsi_Offset offset, size_t *bytesRead) {
  size_t size = offset.end - offset.begin;
  size_t read = 0;
  char *ret = (char*)malloc(size+1);

  if (!(file.tag)) {
    FILE* tmp = fopen(file.filePath, "rb");
    fseek(tmp, offset.begin, SEEK_SET);
    read = fread(ret, 1, size, tmp);
    fseek(tmp, 0, SEEK_SET);
    fclose(tmp);
  } else {
    fseek(file.fileHandle, offset.begin, SEEK_SET);
    read = fread(ret, 1, size, file.fileHandle);
    fseek(file.fileHandle, 0, SEEK_SET);
  }

  if (bytesRead != NULL) *bytesRead = read;
  ret[size] = '\0'; 
  return ret;
}

#endif