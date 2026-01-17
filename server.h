#ifndef SERVER_H_
#define SERVER_H_

#include <strarray.h>

typedef struct {
  StrArray method;
  StrArray path;
  StrArray version;
  StrArray h_keys;
  StrArray h_values;
} http_request;

int http_request_init(http_request *arr);

int http_request_free(http_request *arr);

void http_request_printInfo(http_request *arr);

int parse_first_line(http_request *arr, char **curr_pos, size_t *length);

int parse_headers(http_request *arr, char **curr_pos, size_t *length);

/**
 * @brief Parses a raw http request into StrArray.
 * @param arr Destination StrArray.
 * @param request The request to parse.
 * @param length Length of the request.
 * @return 0
 */
int parse_request(http_request *arr, char *request, size_t length);

#endif // SERVER_H_
