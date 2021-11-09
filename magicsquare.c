/**
 * magicsquare - Generates all NxN magic squares, printing or counting them.
 *
 * Copyright 2021 Carlos Rica (jasampler)
 * This file is the main file of the jasampler's magic-square project.
 * magic-square is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * magic-square is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with the magic-square.  If not, see <https://www.gnu.org/licenses/>.
 */
#define N 5

/** Filter level 0 does not filter the squares,
 * 1 filters equal squares by rotations,
 * 2 also filters equal squares by reflections,
 * 3 also filters equal by interchanging opposite borders and
 * 4 also filters equal by interchanging borders with adjacent rows/colums: */
#define FILTER_LEVEL 4

/** Print style 0 prints only the number of magic squares generated,
 * 1 prints the short reduced version of numbers without spaces printing one
 * line per square and one character for each number, but removing the last
 * column, the last row and the 2nd number in the row previous to the last,
 * 2 prints the long reduced version of numbers without spaces printing one
 * line per square and one character for each number,
 * 3 prints one line per square of decimal numbers separated by commas, and
 * 4 prints rows of decimal numbers separated by spaces ended by empty lines: */
#define PRINT_STYLE 1

/** Fills the holes as soon as possible when it is clear which number goes. */
#define FILL_DERIVED 1

/** Prints the reason to discard the numbers, for debugging. */
#define PRINT_CHECKS 0

#include "sumsquare.c"
#include "sumsquareio.c"
#include "sortednlist.c"
#include "sortednlistsums.c"
#define NDEBUG
#include <assert.h>

#if PRINT_CHECKS
void magicsquare_printchecks(sortednlist nl, sortednlistsums sm, sumsquare sq) {
	int l;
	printf("REMAINING:");
	for (l = sortednlist_first(nl); l; l = sortednlist_next(nl, l)) {
		printf(" %d", l);
	}
	printf("\n");
	printf("MINSUMS:");
	for (l = 0; l < sm->len; l++) {
		printf(" %d", sm->minsums[l]);
	}
	printf(" / MAXSUMS:");
	for (l = 0; l < sm->len; l++) {
		printf(" %d", sm->maxsums[l]);
	}
	printf("\n");
	sumsquare_printsums(sq);
}
#endif

/** Returns if the available numbers could fill the holes of each line to get
 * the magic sum by adding to them the current minimum and maximum sums, and
 * also saves in the given address the index of the first line found with only
 * one hole as candidate to be filled, or -1 if there is no lines with holes. */
char magicsquare_checksums(sumsquare sq, sortednlist nl, sortednlistsums sm,
		int msum, int *pln1hole) {
	int l, nlines = sumsquare_nlines(sq), ln1hole = -1;
	sumsquare_linecount line;
	int *minsums, *maxsums;
	char r = 1;
	sortednlistsums_get(sm, nl);
	minsums = sm->minsums;
	maxsums = sm->maxsums;
	for (l = 0; l < nlines; l++) {
		line = sumsquare_getlinecount(sq, l);
		if (line.holes) {
			assert(line.holes <= sm->len);
			if (line.sum + minsums[line.holes - 1] > msum) {
#if PRINT_CHECKS
printf("INVALID sum=%d holes=%d minsum=%d\n", line.sum, line.holes,
	line.sum + minsums[line.holes - 1]);
magicsquare_printchecks(nl, sm, sq);
#endif
				r = 0;
				break;
			} else if (line.sum + maxsums[line.holes - 1] < msum) {
#if PRINT_CHECKS
printf("INVALID sum=%d holes=%d maxsum=%d\n", line.sum, line.holes,
	line.sum + maxsums[line.holes - 1]);
magicsquare_printchecks(nl, sm, sq);
#endif
				r = 0;
				break;
			} else if (line.holes == 1) {
				if (sortednlist_isremoved(nl, msum - line.sum)){
#if PRINT_CHECKS
printf("INVALID sum=%d holes=1 notavailable=%d\n", line.sum,
	msum - line.sum);
magicsquare_printchecks(nl, sm, sq);
#endif
					r = 0;
					break;
				} else if (ln1hole < 0) {
					ln1hole = l;
				}
			}
		} else if (line.sum != msum) {
#if PRINT_CHECKS
printf("INVALID sum=%d holes=0\n", line.sum);
magicsquare_printchecks(nl, sm, sq);
#endif
			r = 0;
			break;
		}
	}
#if PRINT_CHECKS
if (r) {
	printf("OK!\n");
	magicsquare_printchecks(nl, sm, sq);
}
#endif
	*pln1hole = r && ln1hole > -1 ? ln1hole : -1;
	return r;
}

#define CELLIDXFROMIJ(i, j, side) ((i) * (side) + (j))

/**
 * Returns 0 if the square does not pass the checks for the given filter level.
 *
 * The following magic squares are not generated when filterlevel < 1:
 * Every magic square has other three magic squares obtained by rotating the
 * square. The number in the top-left corner visits the other three corners,
 * so the transformed magic squares in which the number in top-left
 * corner is not the minor one of the three other corners are discarded:
 *     |.1|18|20|24| 2|
 *     |23| 8| 6|12|16|
 *     |19| 3|25| 7|11|
 *     |17|21| 4| 9|14|
 *     | 5|15|10|13|22|
 * Rotate:
 *     | 5|17|19|23|.1|
 *     |15|21| 3| 8|18|
 *     |10| 4|25| 6|20|
 *     |13| 9| 7|12|24|
 *     |22|14|11|16| 2|
 * Rotate:
 *     |22|13|10|15| 5|
 *     |14| 9| 4|21|17|
 *     |11| 7|25| 3|19|
 *     |16|12| 6| 8|23|
 *     | 2|24|20|18|.1|
 * Rotate:
 *     | 2|16|11|14|22|
 *     |24|12| 7| 9|13|
 *     |20| 6|25| 4|10|
 *     |18| 8| 3|21|15|
 *     |.1|23|19|17| 5|
 *
 * The following magic squares are not generated when filterlevel < 2:
 * Every magic square has another magic square obtained by reflecting
 * horizontally the square and rotating it counter-clockwise. Although the
 * numbers in the top-left and bottom-right corners remain equal, the numbers
 * in the top-right and bottom-left corners are interchanged, so the
 * transformed magic squares in which the numbers in the corners of the
 * secondary diagonal are in reverse order will be discarded:
 *     | 1|18|20|24|.2|
 *     |23| 8| 6|12|16|
 *     |19| 3|25| 7|11|
 *     |17|21| 4| 9|14|
 *     |.5|15|10|13|22|
 * Reflect horizontally:
 *     |.2|24|20|18| 1|
 *     |16|12| 6| 8|23|
 *     |11| 7|25| 3|19|
 *     |14| 9| 4|21|17|
 *     |22|13|10|15|.5|
 * Rotate counter-clockwise:
 *     | 1|23|19|17|.5|
 *     |18| 8| 3|21|15|
 *     |20| 6|25| 4|10|
 *     |24|12| 7| 9|13|
 *     |.2|16|11|14|22|
 * Note that applying the transformation again restores the original:
 *     |.5|17|19|23| 1|
 *     |15|21| 3| 8|18|
 *     |10| 4|25| 6|20|
 *     |13| 9| 7|12|24|
 *     |22|14|11|16|.2|
 *
 * The following magic squares are not generated when filterlevel < 3:
 * Every magic square has another magic square with the same numbers in the
 * same corners obtained by interchanging opposite borders and then rotating
 * the square two times. Because of the rotations, the number in the second
 * position of the main diagonal is interchanged with its opposite one, so
 * the transformed magic squares in which the numbers in the interior
 * corners are in reverse order will be discarded:
 *     | 1|18|20|24| 2|
 *     |23|.8| 6|12|16|
 *     |19| 3|25| 7|11|
 *     |17|21| 4|.9|14|
 *     | 5|15|10|13|22|
 * Interchange borders:
 *     |22|15|10|13| 5|
 *     |16|.8| 6|12|23|
 *     |11| 3|25| 7|19|
 *     |14|21| 4|.9|17|
 *     | 2|18|20|24| 1|
 * Rotate x2:
 *     | 1|24|20|18| 2|
 *     |17|.9| 4|21|14|
 *     |19| 7|25| 3|11|
 *     |23|12| 6|.8|16|
 *     | 5|13|10|15|22|
 *
 * The following magic squares are not generated when filterlevel < 4:
 * Every magic square has another magic square obtained by interchanging
 * each border with its adjacent row or column. The number in each corner
 * ends interchanged with the number in its adjacent cell of the same diagonal,
 * so the transformed magic squares in which the minor of all the numbers in
 * the interior and exterior corners is not in the exterior, will be discarded:
 *     |.1|18|20|24| 2|
 *     |23|.8| 6|12|16|
 *     |19| 3|25| 7|11|
 *     |17|21| 4| 9|14|
 *     | 5|15|10|13|22|
 * Interchange borders with adjacent lines:
 *     |.8|23| 6|16|12|
 *     |18|.1|20| 2|24|
 *     | 3|19|25|11| 7|
 *     |15| 5|10|22|13|
 *     |21|17| 4|14| 9| */
char magicsquare_checkequiv(sumsquare sq, char filterlevel) {
	int side, last;
	int topleft, topright, botleft, botright;
	int topleft2, topright2, botleft2, botright2;
	side = sq->side;
	if (filterlevel < 1 || side < 2) {
		return 1;
	}
	last = side - 1;
	topleft = sumsquare_getnum(sq, CELLIDXFROMIJ(0, 0, side));
	topright = sumsquare_getnum(sq, CELLIDXFROMIJ(0, last, side));
	botleft = sumsquare_getnum(sq, CELLIDXFROMIJ(last, 0, side));
	botright = sumsquare_getnum(sq, CELLIDXFROMIJ(last, last, side));
	/* the top-left corner must be less than others
	 * (to discard 3 of 4 rotations): */
	if (topleft && ((topright && topleft > topright)
			|| (botleft && topleft > botleft)
			|| (botright && topleft > botright))) {
#if PRINT_CHECKS
printf("INVALID topleft=%d > topright=%d | botleft=%d | botright=%d\n",
	topleft, topright, botleft, botright);
sumsquare_printsums(sq);
#endif
		return 0;
	}
	if (filterlevel < 2) {
		return 1;
	}
	/* the top-right corner must be less than bottom-left
	 * (to discard 1 of 2 reflections): */
	if (topright && botleft && topright + 1 > botleft) {
#if PRINT_CHECKS
printf("INVALID topright=%d > botleft=%d\n", topright, botleft);
sumsquare_printsums(sq);
#endif
		return 0;
	}
	if (filterlevel < 3 || side < 4) {
		return 1;
	}
	/* the second in the main diagonal must be less than its opposite
	 * (to discard 1 of 2 interchange of borders): */
	topleft2 = sumsquare_getnum(sq, CELLIDXFROMIJ(1, 1, side));
	botright2 = sumsquare_getnum(sq, CELLIDXFROMIJ(last - 1,last - 1,side));
	if (topleft2 && botright2 && topleft2 > botright2) {
#if PRINT_CHECKS
printf("INVALID topleft2=%d > botright2=%d\n", topleft2, botright2);
sumsquare_printsums(sq);
#endif
		return 0;
	}
	if (filterlevel < 4) {
		return 1;
	}
	/* the minor exterior corner must be less than all the interior corners
	 * (to discard 1 of 2 interchange of borders with interior lines): */
	topright2 = sumsquare_getnum(sq, CELLIDXFROMIJ(1, last - 1, side));
	botleft2 = sumsquare_getnum(sq, CELLIDXFROMIJ(last - 1, 1, side));
	if (topleft && ((topleft2 && topleft > topleft2)
			|| (topright2 && topleft > topright2)
			|| (botleft2 && topleft > botleft2))) {
#if PRINT_CHECKS
printf("INVALID topleft=%d > topleft2=%d | topright2=%d | botleft2=%d\n",
		topleft, topleft2, topright2, botleft2);
sumsquare_printsums(sq);
#endif
		return 0;
	}
	return 1;
}

/** Reorders the given list of positions in the square moving to the beginning
 * of the list the positions used to discard equivalent magic squares, as in:
 *    1 . 2    1  .  .  2    1  .  .  .  2
 *    . 5 .    .  5  6  .    .  5  .  6  .
 *    3 . 4    .  7  8  .    .  .  9  .  .
 *             3  .  .  4    .  7  .  8  .
 *                           3  .  .  .  4 */
void magicsquare_initpositionsorder(sortednlist pl, int side) {
	int r, s, i, j, last = side - 1, pos, oldpos = 0;
	int mside = (side / 2) + (side % 2);
	for (r = 0; r < mside; r++) {
		for (s = 0; s < 4; s++) {
			switch (s) {
			case 0: i = r;        j = r;        break;
			case 1: i = r;        j = last - r; break;
			case 2: i = last - r; j = r;        break;
			case 3: i = last - r; j = last - r; break;
			}
			pos = CELLIDXFROMIJ(i, j, side) + 1;
			sortednlist_moveafter(pl, pos, oldpos);
			oldpos = pos;
		}
	}
}

#define MAGSQ_EMPTYPOS 0
#define MAGSQ_TRIEDNUM 1
#define MAGSQ_DERIVEDNUM 2

void magicsquare_initnumtypes(char *arr, int size) {
	int i;
	for (i = 0; i < size; i++) {
		arr[i] = MAGSQ_EMPTYPOS;
	}
}

/** Inserts the given number as a derived number in the given position.
 * A derived number is added when no other number can be in that position,
 * and they must be removed cleanly in the same way that they were added. */
void magicsquare_insertderivednum(sumsquare sq, sortednlist nl, sortednlist pl,
				char *numtypes, int pos, int num) {
#if PRINT_CHECKS
printf("INSERT DERIVED pos=%d num=%d\n", pos, num);
#endif
	assert(numtypes[pos - 1] == MAGSQ_EMPTYPOS);
	assert(! sumsquare_getnum(sq, pos - 1));
	assert(! sortednlist_isremoved(nl, num));
	assert(! sortednlist_isremoved(pl, pos));
	sortednlist_remove(nl, num);
	sumsquare_setnum(sq, pos - 1, num);
	numtypes[pos - 1] = MAGSQ_DERIVEDNUM;
	sortednlist_remove(pl, pos);
}

/** Removes the number in the given position marked as a derived number.
 * A derived number is added when no other number can be in that position,
 * and they must be removed cleanly in the same way that they were added. */
void magicsquare_removederivednum(sumsquare sq, sortednlist nl, sortednlist pl,
				char *numtypes, int pos) {
#if PRINT_CHECKS
printf("REMOVE DERIVED pos=%d num=%d\n", pos, sumsquare_getnum(sq, pos - 1));
#endif
	assert(numtypes[pos - 1] == MAGSQ_DERIVEDNUM);
	assert(sumsquare_getnum(sq, pos - 1));
	assert(sortednlist_nremoved(nl));
	assert(sortednlist_lastremoved(nl) == sumsquare_getnum(sq, pos - 1));
	assert(sortednlist_nremoved(pl));
	assert(pos == sortednlist_lastremoved(pl));
	sortednlist_restore(nl);
	sumsquare_setnum(sq, pos - 1, 0);
	numtypes[pos - 1] = MAGSQ_EMPTYPOS;
	sortednlist_restore(pl);
}

/** Finds and returns the next available number for the given position, being
 * zero if there is not an available number for that position, and writes it in
 * the square updating the list of available numbers and positions. */
int magicsquare_setnext(sumsquare sq, sortednlist nl, sortednlist pl,
			char *numtypes, int pos) {
	int oldnum, num;
	if (numtypes[pos - 1] == MAGSQ_DERIVEDNUM) {
		magicsquare_removederivednum(sq, nl, pl, numtypes, pos);
		return 0;
	}
	oldnum = sumsquare_getnum(sq, pos - 1);
	if (oldnum) {
		assert(oldnum == sortednlist_lastremoved(nl));
		sortednlist_restore(nl);
	}
	num = sortednlist_next(nl, oldnum);
	if (num) {
		sortednlist_remove(nl, num);
		if (! oldnum) {
			assert(numtypes[pos - 1] == MAGSQ_EMPTYPOS);
			assert(! sortednlist_isremoved(pl, pos));
			sortednlist_remove(pl, pos);
			numtypes[pos - 1] = MAGSQ_TRIEDNUM;
		}
	} else if (oldnum) {
		assert(numtypes[pos - 1] == MAGSQ_TRIEDNUM);
		assert(pos == sortednlist_lastremoved(pl));
		sortednlist_restore(pl);
		numtypes[pos - 1] = MAGSQ_EMPTYPOS;
	}
	sumsquare_setnum(sq, pos - 1, num);
	return num;
}

void magicsquare_generate(char filterlevel, char printstyle, char fillderived) {
	unsigned long cricount = 0;
	int pos, msum, auxpos, ln1hole;
	char sqmem[SUMSQUARE_BYTES(N)];
	sumsquare sq = sumsquare_init(sqmem, N);
	char plmem[SORTEDNLIST_BYTES(N * N)];
	sortednlist pl = sortednlist_init(plmem, N * N);
	char nlmem[SORTEDNLIST_BYTES(N * N)];
	sortednlist nl = sortednlist_init(nlmem, N * N);
	char smmem[SORTEDNLISTSUMS_BYTES(N)];
	sortednlistsums sm = sortednlistsums_init(smmem, N);
	unsigned char fixedwidth = sumsquare_fixedwidth(sq, FIXEDWIDTH_BASE);
	char numtypes[N * N];
	magicsquare_initnumtypes(numtypes, N * N);
	magicsquare_initpositionsorder(pl, N);
	pos = sortednlist_first(pl);
	msum = (N * ((N * N) + 1)) / 2;
	while (1) {
		if (magicsquare_setnext(sq, nl, pl, numtypes, pos)) {
			while (magicsquare_checksums(sq, nl, sm, msum, &ln1hole)
				&& magicsquare_checkequiv(sq, filterlevel)) {
				auxpos = sortednlist_first(pl);
				if (auxpos == 0) {
					cricount++;
					if (printstyle == 1) {
						sumsquare_printreduced(sq, 1,
						FIXEDWIDTH_BASE, fixedwidth);
					} else if (printstyle == 2) {
						sumsquare_printreduced(sq, 0,
						FIXEDWIDTH_BASE, fixedwidth);
					} else if (printstyle == 3) {
						sumsquare_printline(sq);
					} else if (printstyle == 4) {
						sumsquare_print(sq);
					}
					break;
				} else if (fillderived && ln1hole > -1) {
					pos = sumsquare_emptycell(sq,ln1hole)+1;
					magicsquare_insertderivednum(sq, nl, pl,
								numtypes, pos,
				msum - sumsquare_getlinecount(sq, ln1hole).sum);
				} else {
					pos = auxpos;
					break;
				}
			}
		} else if (sortednlist_nremoved(pl) == 0) {
			break;
		} else {
			pos = sortednlist_lastremoved(pl);
		}
	}
	if (printstyle == 0) {
		printf("%lu\n", cricount);
	}
}

int main(void) {
	magicsquare_generate(FILTER_LEVEL, PRINT_STYLE, FILL_DERIVED);
	return 0;
}

