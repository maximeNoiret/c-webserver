#include <chararray.h>
#include <strarray.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"

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

int parse_first_line(http_request *arr, char **curr_pos, size_t *length) {
  /* extract method path version */
  // get first line
  char *end_line = memmem(*curr_pos, *length, "\r\n", 2);
  if (!end_line)
    return -1;
  *length -= end_line - *curr_pos + 2;
  // extract
  *end_line = '\0';

  char method[16], target[2048], version[16];
  if (sscanf(*curr_pos, " %15s %2047s %15s", method, target, version) != 3)
    return -1;

  setstr(&arr->method, method);
  setstr(&arr->path, target);
  setstr(&arr->version, version);

  *curr_pos = end_line + 2;
  return 0;
}

int parse_headers(http_request *arr, char **curr_pos, size_t *length) {
  // get headers end pointer
  char *end_of_headers = memmem(*curr_pos, *length, "\r\n\r\n", 4);
  if (!end_of_headers)
    return -1;

  for (char *end_line; *curr_pos < end_of_headers; *curr_pos = end_line + 2) {
    end_line = memmem(*curr_pos, *length, "\r\n", 2);
    char key[64], value[64];
    if (!end_line)
      return -2;
    *length -= end_line - *curr_pos + 2;
    *end_line = '\0';

    if (sscanf(*curr_pos, " %63[^:]: %63[^\r\n]", key, value) != 2)
      return -3;
    addstr(&arr->h_keys, key);
    addstr(&arr->h_values, value);
  }
  return 0;
}

int parse_request(http_request *arr, char *request, size_t length) {
  char *curr_pos = request;
  parse_first_line(arr, &curr_pos, &length);
  int r = parse_headers(arr, &curr_pos, &length);
  return r;
}
