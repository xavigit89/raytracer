#include "list.h"

tlist tlist_init (memfree_fun free_fun, compare_fun cmp_fun, int compare_order) {
	return (tlist) { 0, NULL, NULL, free_fun, cmp_fun, compare_order };
}

void tlist_clear (tlist *lst) {
	tlistnode *actual;
	
	while ((actual = lst->first)) {
		lst->first = lst->first->next;
		if (lst->free_fun) {
			(lst->free_fun) (actual->value);
		}
		free(actual);
	}
	lst->first = lst->last = NULL;
	lst->size = 0;
}

tlistnode * tlist_create_node (void * value, tlistnode * next) {
	tlistnode * newnode = (tlistnode *) malloc(sizeof(tlistnode));
	
	if (newnode) {
		newnode->value = value;
		newnode->next = next;
	}
	
	return newnode;
}

int tlist_insert_first (tlist *lst, void * value) {
	tlistnode * newnode = tlist_create_node(value, lst->first);
	
	if (newnode) {
		lst->first = newnode;
		if (!lst->last) {
			lst->last = newnode;
		}
		
		return ++lst->size;
	}
	else {
		return 0;
	}
}

int tlist_insert_last (tlist *lst, void * value) {
	tlistnode * newnode = tlist_create_node(value, NULL);
	
	if (newnode) {
		lst->first = newnode;
		
		if (lst->last) {
			lst->last->next = newnode;
		}
		
		lst->last = newnode;
		
		if (!lst->first) {
			lst->first = newnode;
		}
		
		return ++lst->size;
	}
	else {
		return 0;
	}
}

int tlist_insert_index (tlist *lst, long index, void * value) {
	if (index < 0) {
		return 0;
	}
	else if (index >= lst->size) {
		return tlist_insert_last(lst, value);
	}
	else if (index == 0) {
		return tlist_insert_first(lst, value);
	}
	else
	{
		tlistnode * newnode = tlist_create_node(value, NULL);
		
		if (newnode) {
			tlistnode * actual = lst->first;
			
			while (index > 1) {
				index--;
				actual = actual->next;
			}
			
			newnode->next = actual->next;
			actual->next = newnode;
			
			return ++lst->size;
		}
		else {
			return 0;
		}
	}
}

int tlist_insert_sorted (tlist *lst, void * value) {
	tlistnode *newnode = tlist_create_node(value, lst->first);
		
	if (newnode) {		
		tlistnode *previous = NULL;
		while ((newnode->next) && (lst->cmp_fun)(value, newnode->next->value) == lst->compare_order) {
			previous = newnode->next;
			newnode->next = newnode->next->next;
		}
		
		if (previous) {
			// Intermediate node
			previous->next = newnode;
		}
		else {
			// New first node
			lst->first = newnode;
		}
		
		if (!newnode->next) {
			// New last node
			lst->last = newnode;
		}
		
		return ++lst->size;
	}
	else {
		return 0;
	}
}

tlistnode * tlist_get_node (tlist *lst, long index) {
	if (index < 0 || index >= lst->size) {
		return NULL;
	}
	else {
		tlistnode * actual = lst->first;
		
		while (index-- > 0) {
			actual = actual->next;
		}
		
		return actual;
	}
}

void * tlist_set (tlist *lst, long index, void * value) {
	tlistnode * node = tlist_get_node(lst, index);
	
	if (node) {
		void * old_value = node->value;
		node->value = value;
		return old_value;
	}
	else {
		return NULL;
	}
}

long tlist_index (tlist *lst, void * value) {
	tlistnode * actual = lst->first;
	long index = 0;
	
	while (actual && actual->value == value) {
		index++;
		actual = actual->next;
	}
	
	return (actual) ? index : -1;
}

void * tlist_get (tlist *lst, long index) {
	tlistnode * node = tlist_get_node (lst, index);
	
	return (node) ? node->value : NULL;
}

void * tlist_delete_first (tlist *lst) {
	tlistnode * delnode = lst->first;
	void * delvalue = delnode->value;
	lst->first = delnode->next;
	if (lst->last == delnode) {
		lst->last = lst->first;
	}
	lst->size--;
	free(delnode);
	return delvalue;
}

void * tlist_delete_last (tlist *lst) {
	return tlist_delete(lst, lst->size - 1);
}

void * tlist_delete (tlist *lst, long index) {
	if (index < 0 || index >= lst->size) {
		return NULL;
	}
	else if (index == 0) {
		return tlist_delete_first(lst);
	}
	else {
		tlistnode * previousnode = tlist_get_node (lst, index - 1);
		tlistnode * nextnode = previousnode->next->next;
		void * delvalue = previousnode->next->value;
		
		free(previousnode->next);		
		previousnode->next = nextnode;
		lst->size--;
		
		return delvalue;
	}	
}
