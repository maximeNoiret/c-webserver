#include <chararray.h>
#include <strarray.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>

#include "server.h"
#include "http_request.h"
#include "pages.h"

int setup_server() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    perror("socket");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    perror("bind");
  if (listen(fd, 16) < 0)
    perror("listen");

  return fd;
} // setup_server

// This function sucks, needs refactoring...
int main_loop(PageArray *pages) {
  int sock_fd = setup_server();
  for (;;) {
    struct sockaddr_in peer;
    socklen_t peerlen = sizeof(peer);
    int conn = accept(sock_fd, (struct sockaddr *)&peer, &peerlen);
    if (conn < 0)
      continue;

    http_request request;
    http_request_init(&request);

    char buf[4096];
    ssize_t n = recv(conn, buf, sizeof(buf), 0);
    int r = parse_request(&request, buf, (size_t)n);
    if (r < 0) {
      close(conn);
      close(sock_fd);
      printf("Error while parsing: %i\n", r);
      exit(-1);
    }

    // http_request_printInfo(&request);

    Page *page;
    CharArray response_code;
    carr_init(&response_code, 16);
    int file;
    page = find_page(pages, request.path.ptr);
    if (page == NULL) {
      file = open("404.html", O_RDONLY);
      setstr(&response_code, "404 Not Found");
    } else {
      file = open(page->file_path.ptr, O_RDONLY);
      setstr(&response_code, "200 OK");
      page_printInfo(page);
    }
    http_request_free(&request);

    struct stat st;
    if (fstat(file, &st) == -1) {
      close(file);
      continue;
    }
    off_t size = st.st_size;

    char hdr[256];
    int hlen = snprintf(hdr, sizeof hdr,
                        "HTTP/1.1 %s\r\n"
                        "Server: c-webserver_PURP\r\n"
                        "Content-Length: %jd\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        response_code.ptr, size);

    free(response_code.ptr);
    if (hlen < 0 || (size_t)hlen >= sizeof hdr || send(conn, hdr, (size_t)hlen, 0) != hlen) {
      close(file);
      close(conn);
      return -1;
    }

    // TODO: send in chunks to respect TCP rules. Too lazy for now.
    off_t offset = 0;
    ssize_t sent = sendfile(conn, file, &offset, (size_t)st.st_size);

    close(file);
    if (sent != st.st_size)
      return -1;
    close(conn);
  }

  close(sock_fd);
  return 0;
} // main_loop



int parse_first_line(http_request *arr, char **curr_pos, size_t *length) {
    if (*length == 0) return -1;

    /* extract method path version */
  // get first line
  char *end_line = memmem(*curr_pos, *length, "\r\n", 2);
  if (!end_line)
    return -1;
  *length -= end_line - *curr_pos + 2;
  // extract
  *end_line = '\0';

  char method[16], target[256], version[16];
  if (sscanf(*curr_pos, " %15s %255s %15s", method, target, version) != 3)
    return -1;

  setstr(&arr->method, method);
  setstr(&arr->path, target);
  setstr(&arr->version, version);

  *curr_pos = end_line + 2;
  return 0;
} // parse_first_line

int parse_headers(http_request *arr, char **curr_pos, size_t *length) {
  // get headers end pointer
  char *end_of_headers = memmem(*curr_pos, *length, "\r\n\r\n", 4);
  if (!end_of_headers)
    return -1;

  for (char *end_line; *curr_pos < end_of_headers; *curr_pos = end_line + 2) {
    end_line = memmem(*curr_pos, *length, "\r\n", 2);
    char key[64], value[2048];
    if (!end_line)
      return -2;
    *length -= end_line - *curr_pos + 2;
    *end_line = '\0';

    if (sscanf(*curr_pos, " %63[^:]: %2047[^\r\n]", key, value) != 2)
      return -3;
    addstr(&arr->h_keys, key);
    addstr(&arr->h_values, value);
  }
  return 0;
} // parse_headers

int parse_request(http_request *arr, char *request, size_t length) {
  char *curr_pos = request;
  if (parse_first_line(arr, &curr_pos, &length) == -1) {
    fprintf(stderr, "parse_first_line failed; first bytes: %.*s\n", (int)length, request);
    return -5;
  }
  int r = parse_headers(arr, &curr_pos, &length);
  return r;
} // parse_request
