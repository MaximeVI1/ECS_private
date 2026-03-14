#include <assert.h>
#include "my_function.h"

int main(int argc, char *argv[]) {
  assert(2 == my_function(1, 1));
  assert(-2 == my_function(-1, -1));
  assert(0 == my_function(0, 0));
  assert(2 == my_function(0, 2));
  //assert(0 == my_function(1, 0));
  // ...
  return(0);
}

