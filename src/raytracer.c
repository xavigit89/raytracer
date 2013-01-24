#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "raytrace.h"

unsigned long byteswap(unsigned long value) {
	return (value << 24) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0x0000FF00) | (value >> 24);
}

int save_to_file(const char* filename, tframe fra) {
	FILE *f = fopen(filename, "wb");
	
	if (f)
	{		
		unsigned long width = byteswap(fra.width);
		unsigned long height = byteswap(fra.height);

		size_t written_bytes = 0;
		
		written_bytes += fwrite (&width, 4, 1, f);
		written_bytes += fwrite (&height, 4, 1, f);
		written_bytes += fwrite (fra.bytes, COLOR_BYTES * sizeof(unsigned char), fra.width * fra.height, f);
		fclose(f);
		
		return written_bytes;
	}
	return 0;
}

int main(int argc, char * argv[]) {
	if (argc > 2)
	{
		twindow wnd;
		tframe fra;
		tvector3d origin;
		tscene scn;	
		
		printf("Reading model...\n");
		if (!parse_model(argv[1], &origin, &wnd, &fra, &scn))
		{
			printf("ERROR: Can't read model XML document: %s\n",argv[1]);
		}

		if (scn.objects.size > 0 && fra.bytes)
		{
			printf("Starting raytracing...\n");
			raytrace(wnd, scn, origin, fra);

			if (save_to_file(argv[2], fra)) {		
				printf("Raytracing complete.\n");
			}
			else {
				printf("Raytracing failed. Cannot save to file.\n");
			}
		}
		else if (scn.objects.size == 0)
		{
			printf("ERROR: no objects where read.\n");
		}
		else
		{
			printf("ERROR: no memory for frame.\n");
		}
		
		if (fra.bytes)
		{
			free(fra.bytes);
			fra.bytes = NULL;
		}
		
		if (scn.objects.first) tlist_clear(&scn.objects);
		
		if (scn.lights.first) tlist_clear(&scn.lights);
		
		assert((!scn.objects.first));
		assert((!scn.lights.first));
		assert((!fra.bytes));
	}
	else
	{
		printf("Usage: test01 <image_filename>\n");
	}
		
	return 0;
}
