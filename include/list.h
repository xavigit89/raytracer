#ifndef __LIST_H
#define __LIST_H

#include <stdlib.h>

typedef int (*compare_fun) (void *, void *);
typedef void (*memfree_fun) (void *);

typedef struct stlistnode {
	void * value;
	struct stlistnode *next;
} tlistnode;

enum eorder { REVERSE = -1, NONREVERSE = 1 };

typedef struct stlist {
	long size;
	tlistnode *first, *last;
	memfree_fun free_fun;
	compare_fun cmp_fun;
	int compare_order;
} tlist;

tlist tlist_init (memfree_fun free_fun, compare_fun cmp_fun, int compare_order);
void tlist_clear (tlist *lst);
int tlist_insert_first (tlist *lst, void * value);
int tlist_insert_last (tlist *lst, void * value);
int tlist_insert_index (tlist *lst, long index, void * value);
int tlist_insert_sorted (tlist *lst, void * value);
void * tlist_set (tlist *lst, long index, void * value);
long tlist_index (tlist *lst, void * value);
void * tlist_get (tlist *lst, long index);
void * tlist_delete_first (tlist *lst);
void * tlist_delete_last (tlist *lst);
void * tlist_delete (tlist *lst, long index);

#endif
