#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"

#define MAX_VECTORS 7
#define MAX_LOOPS 10000000

tvector3d cross_product_v1 (tvector3d v, tvector3d w)
{
	return (tvector3d) { v.y * w.z - v.z * w.y, v.z * w.x - v.x * w.z, v.x * w.y - v.y * w.x };
}

/*
 * This is the worst option, because requires the compiler to create a copy of the resulting vector, and then
 * asigning it to the value of the vector called by reference.*/
/*
void cross_product_v2 (tvector3d * r, tvector3d v, tvector3d w)
{
	*r = (tvector3d) { v.y * w.z - v.z * w.y, v.z * w.x - v.x * w.z, v.x * w.y - v.y * w.x };
}
*/

void cross_product_v3 (tvector3d * r, tvector3d v, tvector3d w)
{
	r->x = v.y * w.z - v.z * w.y;
	r->y = v.z * w.x - v.x * w.z;
	r->z = v.x * w.y - v.y * w.x;
}

tvector3d add_v1(tvector3d *vectors, int n)
{
	tvector3d result = vectors[0];
	for (int i = 1; i < n; i++)
	{
		result = cross_product_v1(result, vectors[i]);
	}
	
	return result;
}

/*
tvector3d add_v2(tvector3d *vectors, int n)
{
	tvector3d result = vectors[0], result2;
	for (int i = 1; i < n; i++)
	{
		cross_product_v2(&result2, result, vectors[i]);
		result = result2;
	}
	
	return result;
}
*/

tvector3d add_v3(tvector3d *vectors, int n)
{
	tvector3d result = vectors[0], result2;
	for (int i = 1; i < n; i++)
	{
		cross_product_v3(&result2, result, vectors[i]);
		result = result2;
	}
	
	return result;
}

int main()
{
	int count = 0;
	tvector3d v;
	tscalar s;
	assert(ZERO(0.0)); count++;
	assert(ZERO(0.0004)); count++;
	assert(ZERO(-0.0004)); count++;
	assert(POSITIVE(0.5)); count++;	
	assert(NEGATIVE(-0.5)); count++;
	assert(!POSITIVE(0.0003)); count++;
	assert(!NEGATIVE(-0.0003)); count++;
	assert(NONPOSITIVE(0.0003)); count++;
	assert(NONNEGATIVE(0.0003)); count++;
	
	v = v_add((tvector3d) { 0.8, 0.0, 0.0 }, (tvector3d) { 0.0, 0.6, 0.0 });
	s = v_norm(v);
	
	assert(v.x == 0.8); count++;
	assert(v.y == 0.6); count++;
	assert(v.z == 0.0); count++;
	assert(ZERO(s - 1.0)); count++;
	
	printf("%d test(s) passed.\n", count);
	
	tvector3d *vectors = (tvector3d *) malloc(MAX_VECTORS * sizeof(tvector3d));
	
	if (vectors)
	{
		tvector3d result1, result2, result3;
		int i, j;
		
		vectors[0] = (tvector3d) { 1.0, 0.0, 0.0 };
		for (i = 1, j = 1; i < MAX_VECTORS; i++, j--)
		{
			if (j < 0) j == 2;
			vectors[i] = (tvector3d) { 0.0, 0.0, 0.0 };
			if (j == 0)
			{
				vectors[i].x = 1.0;
			}
			else if (j == 1)
			{
				vectors[i].y = 1.0;
			}
			else
			{
				vectors[i].z = 1.0;
			}
		}
		
		for (i = 0; i < MAX_LOOPS; i++)
		{
			result1 = add_v1(vectors, MAX_VECTORS);
			// result2 = add_v2(vectors, MAX_VECTORS);
			result3 = add_v3(vectors, MAX_VECTORS);
		}
		
		printf("Method 1: [%.4LF, %.4LF, %.4LF]\n", result1.x, result1.y, result1.z);
		// printf("Method 2: [%.4LF, %.4LF, %.4LF]\n", result2.x, result2.y, result2.z);
		printf("Method 3: [%.4LF, %.4LF, %.4LF]\n", result3.x, result3.y, result3.z);
		
		free(vectors);
	}	
	
	return 0;
}
