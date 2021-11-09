/**
 * sortednlist - Sorted double-linked list in static memory storing all integers
 * from 1 to a given constant, implemented as a circular list with cursors, that
 * supports removing numbers, restore each removed number from a stack, get if a
 * number is currently removed or not and get in order the numbers not removed
 * from first to last or from last to first. To create a list of numbers 1 to N,
 * a char array of size SORTEDNLIST_BYTES(N) must be initialized by calling to
 * sortednlist_init(array, N) which returns the same array of type sortednlist.
 * The list can be reordered by moving numbers when there are not removed ones.
 *
 * Copyright 2021 Carlos Rica (jasampler)
 * This file is part of the jasampler's magic-square project.
 */
#include <assert.h>

#define SORTNL_BOOL char
#define SORTNL_TRUE 1
/* unsigned char supports a maximum of N=255 */
#define SORTNL_TYPE unsigned char
#define SORTNL_INDEX(idx) ((int) idx)

typedef struct sortednlist_st {
	SORTNL_TYPE size, nremoved, *stack;
	struct sortednlist_elem_st* elems;
} *sortednlist;

struct sortednlist_elem_st {
	SORTNL_BOOL isremoved;
	SORTNL_TYPE next, prev;
};

#define SORTEDNLIST_BYTES(size) \
	(sizeof(struct sortednlist_st) \
		+ ((size) * sizeof(SORTNL_TYPE)) \
		+ ((size + 1) * sizeof(struct sortednlist_elem_st)))

/** Must receive as arguments an array of SORTNL_TYPE(N) bytes and the same
 * number N, and it returns the same array initalized as a sortednlist list. */
sortednlist sortednlist_init(char *mem, SORTNL_TYPE size) {
	SORTNL_TYPE i;
	sortednlist nl = (sortednlist) mem;
	SORTNL_TYPE *stack = (SORTNL_TYPE *) (nl + 1);
	struct sortednlist_elem_st *elems = (struct sortednlist_elem_st *)
		(stack + size);
	nl->size = size;
	nl->nremoved = 0;
	nl->stack = stack;
	nl->elems = elems;
	for (i = size; i > 0; i--) { /* reverse to protect the end condition */
		elems[SORTNL_INDEX(i)].isremoved = ! SORTNL_TRUE;
		elems[SORTNL_INDEX(i)].prev = i - 1;
		elems[SORTNL_INDEX(i)].next = i + 1;
	}
	elems[SORTNL_INDEX(size)].next = 0;
	elems[0].isremoved = SORTNL_TRUE;
	elems[0].prev = size;
	elems[0].next = 1;
	return nl;
}

/** Returns the constant size of the list. */
#define sortednlist_size(l) ((l)->size)

/** Returns the number of currently removed numbers, from 0 (empty) to size. */
#define sortednlist_nremoved(l) ((l)->nremoved)

/** Returns the last removed number in the stack, which must not be empty. */
#define sortednlist_lastremoved(l) ((l)->stack[(l)->nremoved - 1])

/** Returns the first number currently not removed and the smallest one. */
#define sortednlist_first(l) ((l)->elems[0].next)

/** Returns the last number currently not removed and the biggest one. */
#define sortednlist_last(l) ((l)->elems[0].prev)

/** Returns 0 if the given number is not in the stack of removed numbers. */
#define sortednlist_isremoved(l, n) ((l)->elems[SORTNL_INDEX(n)].isremoved)

/** Returns 0 when the given number does not have a next number. */
#define sortednlist_next(l, n) ((l)->elems[SORTNL_INDEX(n)].next)

/** Returns 0 when the given number does not have a previous number. */
#define sortednlist_prev(l, n) ((l)->elems[SORTNL_INDEX(n)].prev)

/** Moves a given number to be after another given number changing the order of
 * the list but without changing the direct access to the elements by number.
 * Returns 0 if the stack of removed numbers is not empty.
 * Before: ... n->prev <==> n <==> n->next ... p <=========> p->next ...
 * After:  ... n->prev <=========> n->next ... p <==> n <==> p->next ... */
int sortednlist_moveafter(sortednlist nl, SORTNL_TYPE n, SORTNL_TYPE p) {
	struct sortednlist_elem_st *nelem, *pelem;
	assert(n < nl->size + 1 && p < nl->size + 1);
	nelem = nl->elems + n;
	pelem = nl->elems + p;
	if (nl->nremoved) {
		return 0;
	}
	if (n == p || nelem->prev == p) {
		return 1;
	}
	sortednlist_next(nl, nelem->prev) = nelem->next;
	sortednlist_prev(nl, nelem->next) = nelem->prev;
	nelem->prev = p;
	nelem->next = pelem->next;
	sortednlist_prev(nl, pelem->next) = n;
	pelem->next = n;
	return 1;
}

/** Remove the given number from the list moving it to the stack of removed
 * numbers returning 0 if that number is already removed. */
SORTNL_TYPE sortednlist_remove(sortednlist nl, SORTNL_TYPE n) {
	struct sortednlist_elem_st *elem;
	assert(n < nl->size + 1);
	elem = nl->elems + n;
	if (elem->isremoved) {
		return 0;
	}
	elem->isremoved = SORTNL_TRUE;
	nl->stack[SORTNL_INDEX(nl->nremoved++)] = n;
	sortednlist_next(nl, elem->prev) = elem->next;
	sortednlist_prev(nl, elem->next) = elem->prev;
	return n;
}

/** Restore the most recent removed number and returns it or returns 0
 * if the stack of removed numbers is currently empty.*/
SORTNL_TYPE sortednlist_restore(sortednlist nl) {
	if (nl->nremoved) {
		int n = nl->stack[SORTNL_INDEX(--(nl->nremoved))];
		struct sortednlist_elem_st *elem = nl->elems + n;
		elem->isremoved = ! SORTNL_TRUE;
		sortednlist_next(nl, elem->prev) = n;
		sortednlist_prev(nl, elem->next) = n;
		return n;
	}
	return 0;
}

