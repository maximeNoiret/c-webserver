
#include <stdlib.h>
#include <strarray.h>
#include <string.h>


#include "pages.h"
#include "server.h"

void generatePages(PageArray *arr) {
  pagearr_addPage(arr, "/user", "UserPage.html");
  pagearr_addPage(arr, "/about", "AboutUs.html");
  pagearr_addPage(arr, "/", "index.html");
}

int main() {
  PageArray testPages;
  pagearr_init(&testPages, 3);
  generatePages(&testPages);
  pagearr_printInfo(&testPages);
  // test request to make parser
  return main_loop(&testPages);
}
