#include <stdio.h>
#include <stdlib.h>
#include <strarray.h>
#include <string.h>

#include "server.h"

int main() {
  // test request to make parser
  char buf[] = "GET /index.html HTTP/1.1\r\n"
               "Host: example.com\r\n"
               "User-Agent: curl/8.4.0\r\n"
               "Accept: text/html\r\n"
               "Connection: close\r\n"
               "\r\n";
  size_t n = sizeof(buf) - 1; // this came from recv shh
  http_request request;
  http_request_init(&request);

  int r = parse_request(&request, buf, n);
  printf("Return: %i\n", r);

  printf("Test\n");
  http_request_printInfo(&request);

  http_request_free(&request);
  return 0;
}
