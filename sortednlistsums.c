/**
 * sortednlistsums - Pack of two arrays to save the sums of the N first/last
 * numbers of a sortednlist. To create the arrays for N numbers, a char array
 * of size SORTEDNLISTSUMS_BYTES(N) must be initialized by calling to
 * sortednlistsums_init(array N) that returns the array of type sortednlistsums.
 *
 * Copyright 2021 Carlos Rica (jasampler)
 * This file is part of the jasampler's magic-square project.
 */
typedef struct sortednlistsums_st {
	int size, len, *minsums, *maxsums;
} *sortednlistsums;

#define SORTEDNLISTSUMS_BYTES(size) \
	(sizeof(struct sortednlistsums_st) \
		+ ((size + size) * sizeof(int)))

/** Must receive as arguments an array of SORTEDNLISTSUMS_BYTES(N) bytes and the
 * same number N, and returns the same array initalized as a sortednlistsums. */
sortednlistsums sortednlistsums_init(char *mem, int size) {
	sortednlistsums sm = (sortednlistsums) mem;
	int *minsums = (int *) (sm + 1);
	int *maxsums = minsums + size;
	sm->size = size;
	sm->len = 0;
	sm->minsums = minsums;
	sm->maxsums = maxsums;
	return sm;
}

/** Saves in the minimum array the partial sums of the first numbers and
 * saves in the maximum array the partial sums of the last numbers. The index 0
 * saves the first/last number, the index 1 the sum of the 2 first/last numbers,
 * the index 2 the sum of 3, and so on, saving in len the number of saved sums,
 * that can be less than the size when there are less numbers in the list. */
void sortednlistsums_get(sortednlistsums sm, sortednlist nl) {
	int l, min, max, minsum = 0, maxsum = 0;
	min = sortednlist_first(nl);
	max = sortednlist_last(nl);
	for (l = 0; l < sm->size && min; l++) {
		minsum += min;
		maxsum += max;
		sm->minsums[l] = minsum;
		sm->maxsums[l] = maxsum;
		min = sortednlist_next(nl, min);
		max = sortednlist_prev(nl, max);
	}
	sm->len = l;
}

