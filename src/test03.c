#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"

#define MAX_STRUCTS 7
#define MAX_LOOPS 10000000

typedef struct {
	long x, y, z;
	long *p, *q, *r;
} astruct;

/*
 * Due to similarity in performance of methods 1 and 2, I choose method
 * 1 because its simplicity, and clarity.
 * */
astruct cross_product_v1 (astruct v, astruct w)
{
	return (astruct) { v.y * w.z - v.z * w.y, v.z * w.x - v.x * w.z, v.x * w.y - v.y * w.x , NULL, NULL, NULL };
}

void cross_product_v2 (astruct * r, astruct v, astruct w)
{
	r->x = v.y * w.z - v.z * w.y;
	r->y = v.z * w.x - v.x * w.z;
	r->z = v.x * w.y - v.y * w.x;
	r->p = r->q = r->r = NULL;
}

astruct add_v1(astruct *structs, int n)
{
	astruct result = structs[0];
	for (int i = 1; i < n; i++)
	{
		result = cross_product_v1(result, structs[i]);
	}
	
	return result;
}

astruct add_v2(astruct *structs, int n)
{
	astruct result = structs[0], result2;
	for (int i = 1; i < n; i++)
	{
		cross_product_v2(&result2, result, structs[i]);
		result = result2;
	}
	
	return result;
}

int main()
{
	astruct *structs = (astruct *) malloc(MAX_STRUCTS * sizeof(astruct));
	
	if (structs)
	{
		astruct result1, result2;
		int i, j;
		
		structs[0] = (astruct) { 1, 0, 0 , NULL, NULL, NULL };
		for (i = 1, j = 1; i < MAX_STRUCTS; i++, j--)
		{
			if (j < 0) j == 2;
			structs[i] = (astruct) (astruct) { 0, 0, 0 , NULL, NULL, NULL };
			if (j == 0)
			{
				structs[i].x = 1;
			}
			else if (j == 1)
			{
				structs[i].y = 1;
			}
			else
			{
				structs[i].z = 1;
			}
		}
		
		for (i = 0; i < MAX_LOOPS; i++)
		{
			result1 = add_v1(structs, MAX_STRUCTS);
			result2 = add_v2(structs, MAX_STRUCTS);
		}
		
		printf("Method 1: [%4ld, %4ld, %4ld]\n", result1.x, result1.y, result1.z);
		printf("Method 2: [%4ld, %4ld, %4ld]\n", result2.x, result2.y, result2.z);
		
		free(structs);
	}	
	
	return 0;
}
