#ifndef SERVER_H_
#define SERVER_H_

#include <strarray.h>

/**
 * @brief Parses a raw http request into StrArray.
 * @param arr Destination StrArray.
 * @param request The request to parse.
 * @param length Length of the request.
 * @return 0
 */
int parse_request(StrArray *arr, char *request, size_t length);

#endif // SERVER_H_
