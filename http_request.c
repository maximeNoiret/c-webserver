#include <stdlib.h>

#include "http_request.h"


int http_request_init(http_request *arr) {
  carr_init(&arr->method, 3);
  carr_init(&arr->path, 16);
  carr_init(&arr->version, 8);
  sarr_init(&arr->h_keys, 0);
  sarr_init(&arr->h_values, 0);
  return 0;
}

int http_request_free(http_request *arr) {
  free(arr->method.ptr);
  free(arr->path.ptr);
  free(arr->version.ptr);
  sarr_free(&arr->h_keys);
  sarr_free(&arr->h_values);
  return 0;
}

void http_request_printInfo(http_request *arr) {
  printf("HTTP Request:\n  Method: %s\n  Path: %s\n  Version: %s\n  Headers:\n",
         arr->method.ptr, arr->path.ptr, arr->version.ptr);
  for (size_t i = 0; i < arr->h_keys.length; ++i) {
    printf("    %s: %s\n", arr->h_keys.ptr[i].ptr, arr->h_values.ptr[i].ptr);
  }
}