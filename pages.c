#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pages.h"

void page_printInfo(Page *page) {
  printf("    URI: %s\n    PATH: %s\n", page->uri.ptr, page->file_path.ptr);
}
void pagearr_printInfo(PageArray *arr) {
  printf("Pages:\n  Page Count: %zu\n  Allocated: %zu\n  Pages:\n", arr->n,
         arr->c);
  for (size_t i = 0; i < arr->n; ++i) {
    page_printInfo(&arr->ptr[i]);
  }
}

int page_init(Page *page) {
  carr_init(&page->uri, 0);
  carr_init(&page->file_path, 0);
  return 0;
} // page_init

int page_free(Page *page) {
  free(page->uri.ptr);
  free(page->file_path.ptr);
  return 0;
} // page_free

int pagearr_init(PageArray *arr, size_t c) {
  arr->n = 0;
  arr->c = (c ? c : 1);
  arr->ptr = malloc(sizeof(Page) * arr->c);
  if (!arr->ptr)
    return -1;

  return 0;
}

int pagearr_free(PageArray *arr) {
  for (size_t i = 0; i < arr->n; ++i) {
    page_free(&arr->ptr[i]);
  }

  free(arr->ptr);
  return 0;
}

static int grow(PageArray *arr, size_t need) {
  if (need <= arr->c)
    return 0;
  size_t new_c = arr->c;
  while (new_c < need)
    new_c *= 2;

  Page *tmp = realloc(arr->ptr, sizeof(Page) * new_c);
  if (!tmp)
    return -1;
  arr->ptr = tmp;
  arr->c = new_c;
  return 0;
}

int pagearr_addPage(PageArray *arr, char *uri, char *filePath) {
  grow(arr, arr->n + 1);
  page_init(&arr->ptr[arr->n++]);
  setstr(&arr->ptr[arr->n - 1].uri, uri);
  setstr(&arr->ptr[arr->n - 1].file_path, filePath);
  return 0;
}

int find_page(PageArray *arr, Page *page, const char *uri) {
  for (size_t i = 0; i < arr->n; ++i) {
    if (arr->ptr[i].uri.length != strlen(uri))
      continue;
    if (strncmp(arr->ptr[i].uri.ptr, uri, arr->ptr[i].uri.length) == 0) {
      // found page
      *page = arr->ptr[i];
      return 0;
    }
  }
  return -1;
}
