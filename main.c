#include <stdio.h>
#include <stdlib.h>
#include <strarray.h>
#include <string.h>

#include "server.h"

int main() {
  // test request to make parser
  char buf[] =
    "GET /search?q=widgets&page=2 HTTP/1.1\r\n"
    "Host: api.example.com\r\n"
    "User-Agent: curl/8.4.0\r\n"
    "Accept: application/json\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";
  size_t n = sizeof(buf) - 1;  // this came from recv shh
  StrArray arr;
  sarr_init(&arr, 3);
  if (parse_request(&arr, buf, n) == -1) {
    printf("FUCK\n");
    exit(-1);
  }

  printf("Test\n");
  sarr_printInfo(&arr, 0);

  sarr_free(&arr);
  return 0;
}
