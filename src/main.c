#include <stdio.h>

#include "yokai/repl.h"

int main(void) {
  repl_start(stdin, stdout);

  return 0;
}
