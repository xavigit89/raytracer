#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

char buffer[BUFFER_SIZE + 1];

int is_blank_line(char *s)
{
	return !strncmp(s, "", BUFFER_SIZE);
}

int parse_document(char *filename)
{
	
	FILE *f = fopen(filename,"r");
	
	if (f)
	{		
		// Skipping blank lines
		
		// parse_scene(f);		
		fclose(f);
		
		return 1;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{	
	if (argc > 1)
	{
		if (parse_document(argv[1]))
		{
			printf("Document parsing complete.\n");
		}
		else
		{
			printf("ERROR: Document parsing failed.\n");
		}
	}
	else
	{
		printf("Usage: protoIO <filename>\n");
	}
	
	return 0;
}
