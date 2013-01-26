#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "raytrace.h"
#include "xmlio.h"
#include "avsio.h"

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
			
			return -1;
		}

		if (scn.objects.size > 0 && fra.bytes)
		{
			printf("Starting raytracing...\n");
			raytrace(wnd, scn, origin, fra);

			if (save_to_file(argv[2], fra.width, fra.height, fra.bytes)) {		
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
		
		tscene_clear(&scn);
		
		if (fra.bytes)
		{
			free(fra.bytes);
			fra.bytes = NULL;
		}
			
		assert((!scn.objects.first));
		assert((!scn.lights.first));
		assert((!fra.bytes));
	}
	else
	{
		printf("Usage: raytracer <input_model> <output_image>\n");
	}
		
	return 0;
}
