#include <stdio.h>
#include <stdlib.h>
#include <strarray.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "server.h"
#include "pages.h"

void generatePages(PageArray *arr) {
  pagearr_addPage(arr, "/user", "UserPage.html");
  pagearr_addPage(arr, "/about", "AboutUs.html");
  pagearr_addPage(arr, "/", "index.html");
}

int main() {
  PageArray testPages;
  pagearr_init(&testPages, 3);
  generatePages(&testPages);
  page_printInfo(&testPages.ptr[2]);
  // test request to make parser
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) perror("socket");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(8080);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    perror("bind");
  if (listen(fd, 16) < 0)
    perror("listen");

  for (;;) {
    struct sockaddr_in peer;
    socklen_t peerlen = sizeof(peer);
    int conn = accept(fd, (struct sockaddr *)&peer, &peerlen);
    if (conn < 0) continue;
    
    http_request request;
    http_request_init(&request);

    char buf[4096];
    ssize_t n = recv(conn, buf, sizeof(buf), 0); 
    int r = parse_request(&request, buf, (size_t)n);
    if (r < 0) {
      close(conn);
      close(fd);
      printf("Error while parsing: %i\n", r);
      exit(-1);
    }
    
    //http_request_printInfo(&request);

    Page page;
    CharArray response_code;
    carr_init(&response_code, 16);
    page_init(&page);
    int file;
    if (find_page(&testPages, &page, request.path.ptr) == -1) {
      file = open("404.html", O_RDONLY);
      setstr(&response_code, "404 Not Found");
    } else {
      file = open(page.file_path.ptr, O_RDONLY);
      setstr(&response_code, "200 OK");
    }
    page_printInfo(&page);
    struct stat st;
    if (fstat(file, &st) == -1) { close(file); continue; }
    off_t size = st.st_size;

    char hdr[256];
    int hlen = snprintf(hdr, sizeof hdr,
        "HTTP/1.1 %s\r\n"
        "Server: c-webserver_PURP\r\n"
        "Content-Length: %jd\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n", response_code.ptr, size);
    if (hlen < 0 || (size_t)hlen >= sizeof hdr) { close(file); return -1; }

    if (send(conn, hdr, (size_t)hlen, 0) != hlen) { close(file); return -1; }

    off_t offset = 0;
    ssize_t sent = sendfile(conn, file, &offset, (size_t)st.st_size);
    close(file);
    if (sent != st.st_size) return -1;
    
    close(conn);

    http_request_free(&request);
  }

  /*char buf[] = "GET /index.html HTTP/1.1\r\n"
               "Host: example.com\r\n"
               "User-Agent: curl/8.4.0\r\n"
               "Accept: text/html\r\n"
               "Connection: close\r\n"
               "\r\n";
  size_t n = sizeof(buf) - 1; // this came from recv shh*/
  
  return 0;
}
