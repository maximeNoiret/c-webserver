#ifndef PAGES_H_
#define PAGES_H_

#include <chararray.h>

typedef struct {
  CharArray uri;
  CharArray file_path;
} Page;

typedef struct {
  size_t n;
  size_t c;
  Page *ptr;
} PageArray;


void page_printInfo(Page *page, size_t level);
void pagearr_printInfo(PageArray *arr, size_t level);

int page_init(Page *page);
int page_free(Page *page);
int pagearr_init(PageArray *arr, size_t c);
int pagearr_free(PageArray *arr);

int pagearr_addPage(PageArray *arr, char *uri, char *filePath);


#endif // PAGES_H_