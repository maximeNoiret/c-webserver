#include <stdlib.h>
#include <strarray.h>
#include <string.h>


#include "pages.h"
#include "server.h"

void generatePages(PageArray *arr) {
  pagearr_addPage(arr, "/about", "AboutUs.html");
  pagearr_addPage(arr, "/", "index.html");
}

int main() {
  PageArray testPages;
  pagearr_init(&testPages, 3);
  generatePages(&testPages);
  pagearr_printInfo(&testPages);
  // test request to make parser
  int ret_val = main_loop(&testPages);

  pagearr_free(&testPages);

  return ret_val;
}
