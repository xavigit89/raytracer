#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

double * double_new() {
	return (double *) malloc(sizeof(double));
}

int double_cmp(void *a, void *b) {
	double x = *((double *) a);
	double y = *((double *) b);
	return (x == y) ? 0 : ((x > y) ? 1 : -1);
}

int main() {
	int count = 0, i;
	double * value;
	unsigned short state[3] = { 0, 0, 37 };
	tlist lst = tlist_init(free, NULL, REVERSE);
	assert(lst.free_fun == free); count++;
	assert(lst.cmp_fun == NULL); count++;
	lst.cmp_fun = double_cmp;
	lst.compare_order = REVERSE;
	srand(time(NULL));
	
	for (i = 0; i < 15; i++) {
		value = double_new();
		
		if (value) {
			*value = erand48(state);
			printf("%.4lf ",*value);
			tlist_insert_sorted(&lst, value);
		}
	}
	value = double_new();
	*value = -3.25;
	tlist_insert_first(&lst, value);
	value = double_new();
	*value = 4.91;
	tlist_insert_first(&lst, value);
	printf("\n\n");
	
	tlistnode *node = lst.first;
	
	while (node) {
		printf("%.4lf ",*((double *) node->value));
		node = node->next;
	}
	printf("\n\n");
	
	tlist_clear(&lst);
	assert(lst.size == 0); count++;
	assert(!lst.first); count++;
	assert(!lst.last); count++;
	printf("%d test(s) passed.\n",count);
	return 0;
}
