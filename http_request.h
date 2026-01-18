#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <chararray.h>
#include <strarray.h>

typedef struct {
  CharArray method;
  CharArray path;
  CharArray version;
  StrArray h_keys;
  StrArray h_values;
} http_request;

int http_request_init(http_request *arr);

int http_request_free(http_request *arr);

void http_request_printInfo(http_request *arr);


#endif // HTTP_REQUEST_H_