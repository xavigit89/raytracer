#include "avsio.h"

unsigned long byteswap(unsigned long value) {
	return (value << 24) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0x0000FF00) | (value >> 24);
}

int save_to_file(const char* filename, unsigned long width, unsigned long height, unsigned char *bytes)
{
	FILE *f = fopen(filename, "wb");
	
	if (f)
	{		
		unsigned long swap_width = byteswap(width);
		unsigned long swap_height = byteswap(height);

		size_t written_bytes = 0;
		
		written_bytes += fwrite (&swap_width, 4, 1, f);
		written_bytes += fwrite (&swap_height, 4, 1, f);
		written_bytes += fwrite (bytes, COLOR_BYTES * sizeof(unsigned char), width * height, f);
		fclose(f);
		
		return written_bytes;
	}
	return 0;
}
