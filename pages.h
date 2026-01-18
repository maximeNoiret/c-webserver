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


void page_printInfo(Page *page);
void pagearr_printInfo(PageArray *arr);

int page_init(Page *page);
int page_free(Page *page);
int pagearr_init(PageArray *arr, size_t c);
int pagearr_free(PageArray *arr);

int pagearr_addPage(PageArray *arr, char *uri, char *filePath);

int find_page(PageArray *arr, Page *page, const char *uri);


#endif // PAGES_H_