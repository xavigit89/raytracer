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
		
		parse_model(argv[2], &origin, &wnd, &fra, &scn);
		
		/*if (fra.bytes) free(fra.bytes);
		
		printf("Creating scene elements...\n");
		
		wnd = (twindow) { (tvector3d) {-800.0, -450.0, 0.0}, 1600.0, 900.0 };
		fra = (tframe) { 800, 600, (unsigned char *) malloc(COLOR_BYTES * 800 * 600 * sizeof(unsigned char)) };
		origin = (tvector3d) { 0.0, 0.0, -10000.0};

		tlight *light1 = tlight_new();
		tobject *obj1 = tobject_new();
		tsphere *sph1 = tsphere_new();
		*/
		/*
		if ((light1) && (obj1) && (sph1)) {
			printf("Initializing elements...\n");
			*light1 = tlight_init((tvector3d) { -300.0, 0.0, -400.0 }, (tvector4d) { 1.0, 1.0, 1.0, 1.0 }, 1.0, 1.0, 0.0, 0.0);
			*obj1 = tobject_init((tvector4d) { 1.0, 0.0, 0.0, 1.0 },
								 1.0, 1.0, 0.8, 4,
								 tsphere_intersections, tsphere_normal, free);
			
			*sph1 = tsphere_init((tvector3d) { 0.0, 0.0, 0.0 }, 100.0);
			obj1->properties = sph1;
			tscene_init(&scn, (tvector4d) { 0.5, 0.5, 0.5, 1.0 }, 0.15);
				
			tlist_insert_first(&scn.objects, obj1);
			tlist_insert_first(&scn.lights, light1);
			
			printf("Starting raytracing...\n");
			
			assert(scn.objects.size == 1);
			assert(scn.lights.size == 1);
			*/
		if (scn.objects.size > 0 && fra.bytes)
		{
			raytrace(wnd, scn, origin, fra);

			if (save_to_file(argv[1], fra)) {		
				printf("Raytracing complete.\n");
			}
			else {
				printf("Raytracing failed. Cannot save to file.\n");
			}			
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
