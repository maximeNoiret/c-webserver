#include <chararray.h>
#include <strarray.h>
#include <string.h>

#include "server.h"

int parse_request(StrArray *arr, char *request, size_t length) {

  char *curr_pos = request;
  // get first line
  char *end_line = memmem(request, length, "\r\n", 2);
  if (!end_line)
    return -1;
  length -= end_line - curr_pos + 2;
  // extract
  *end_line = '\0';

  char method[16], target[2048], version[16];
  if (sscanf(curr_pos, " %15s %2047s %15s", method, target, version) != 3) {
    return -1;
  } else {
    addstr(arr, method);
    addstr(arr, target);
    addstr(arr, version);
    return 0;
  }

  return 0;
}
