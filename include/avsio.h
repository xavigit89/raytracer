#include <stdio.h>
#include "objects.h"

unsigned long byteswap(unsigned long value);
int save_to_file(const char* filename, unsigned long width, unsigned long height, unsigned char *bytes);
