#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "huffman.h"

int main()
{
  const char *s = "aaaaabccccccddd";

  huffman_encode(s);

  return 0;
}
