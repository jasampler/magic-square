/**
 * sumsquareio - Functions for printing magic squares of type sumsquare.
 *
 * Copyright 2021 Carlos Rica (jasampler)
 * This file is part of the jasampler's magic-square project.
 */
#include <stdio.h>

static char DIGITS[] =
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz{}";
#define MAX_DIGITS (sizeof(DIGITS))
#define FIXEDWIDTH_BASE MAX_DIGITS

void sumsquare_print(sumsquare sq) {
	int i, j, side = sumsquare_side(sq);
	char *fmt;
	if (side * side < 10) {
		fmt = " %d";
	} else if (side * side < 100) {
		fmt = " %2d";
	} else {
		fmt = " %3d";
	}
	for (i = 0; i < side; i++) {
		for (j = 0; j < side; j++) {
			printf(fmt + (j ? 0 : 1),
				sumsquare_getnum(sq, i * side + j));
		}
		printf("\n");
	}
	printf("\n");
}

void sumsquare_printline(sumsquare sq) {
	int p, n, sidexside = sumsquare_side(sq);
	sidexside *= sidexside;
	for (p = 0; p < sidexside; p++) {
		if (p) {
			printf(",");
		}
		n = sumsquare_getnum(sq, p);
		if (n) {
			printf("%d", n);
		}
	}
	printf("\n");
}

/* Prints a number with fixed width, adding zeros at the beginning if needed. */
void sumsquare_printfixedwidth(int n, unsigned char base,
				unsigned char fixedwidth) {
	unsigned char nwidth;
	int pow, m;
	if (n > base - 1) {
		for (m = n / base, nwidth = 1, pow = 1; m;
				nwidth++, pow *= base, m /= base) { }
		for (; fixedwidth > nwidth; fixedwidth--) {
			putchar(DIGITS[0]);
		}
		do {
			putchar(DIGITS[n / pow]);
			n %= pow;
			pow /= base;
		} while (pow > 1);
	} else {
		for (; fixedwidth > 1; fixedwidth--) {
			putchar(DIGITS[0]);
		}
	}
	putchar(DIGITS[n]);
}

/** Returns the width in the given base of the biggest number of the square. */
unsigned char sumsquare_fixedwidth(sumsquare sq, unsigned char base) {
	unsigned char nwidth;
	int m;
	for (m = sumsquare_side(sq) * sumsquare_side(sq) / base, nwidth = 1; m;
			nwidth++, m /= base) { }
	return nwidth;
}

/** Prints the numbers of the square with fixed width and without separation.
 * The short format removes the last column, last row and the 2nd number in the
 * row previous to the last, since these can be calculated in magic squares. */
void sumsquare_printreduced(sumsquare sq, char shortformat, unsigned char base,
						unsigned char fixedwidth) {
	int i, j, side = sumsquare_side(sq);
	int maxline = shortformat ? side - 1 : side;
	int skipi = shortformat ? side - 2 : -1;
	int skipj = shortformat ? 1 : -1;
	for (i = 0; i < maxline; i++) {
		for (j = 0; j < maxline; j++) {
			if (i != skipi || j != skipj) {
				sumsquare_printfixedwidth(sumsquare_getnum(sq,
					i * side + j), base, fixedwidth);
			}
		}
	}
	putchar('\n');
}

#define ROWIDX(sq, i) SUMSQUARE_ROWIDX(sq, i)
#define COLIDX(sq, j) SUMSQUARE_COLIDX(sq, j)
#define DIAGIDX(sq, d) SUMSQUARE_DIAGIDX(sq, d)

void sumsquare_printsums(sumsquare sq) {
	int i, j;
	int side = sumsquare_side(sq);
	for (i = 0; i < side; i++) {
		printf("  ");
		for (j = 0; j < side; j++) {
			printf(" %2d", sumsquare_getnum(sq, side * i + j));
		}
		printf(" -- %2d (%d)\n",
			sumsquare_getlinecount(sq, ROWIDX(sq, i)).sum,
			sumsquare_getlinecount(sq, ROWIDX(sq, i)).holes);
	}
	printf("  / |");
	for (j = 1; j < side; j++) {
		printf("  |");
	}
	printf(" \\ \n");
	printf("%2d", sumsquare_getlinecount(sq, DIAGIDX(sq, 1)).sum);
	for (j = 0; j < side; j++) {
		printf(" %2d", sumsquare_getlinecount(sq, COLIDX(sq, j)).sum);
	}
	printf(" %2d\n", sumsquare_getlinecount(sq, DIAGIDX(sq, 0)).sum);
	printf("(%d)", sumsquare_getlinecount(sq, DIAGIDX(sq, 1)).holes);
	for (j = 0; j < side; j++) {
		printf("(%d)",
			sumsquare_getlinecount(sq, COLIDX(sq, j)).holes);
	}
	printf("(%d)\n\n", sumsquare_getlinecount(sq, DIAGIDX(sq, 0)).holes);
}

