/**
 * sumsquare - Table with NxN cells that allows storing positive integers on it
 * and maintains the sum of each "line", currently rows, columns and diagonals,
 * maintaining also the number of empty holes in each line. To create a
 * table of NxN, a char array of size SUMSQUARE_BYTES(N) must be initialized by
 * calling to sumsquare_init(array, N) that returns the array of type sumsquare.
 *
 * Copyright 2021 Carlos Rica (jasampler)
 * This file is part of the jasampler's magic-square project.
 */
/* unsigned char supports only integers from 0 to 255 */
#define SUMSQ_NRELTYPE unsigned char
#define SUMSQ_NUMTYPE unsigned char
#define SUMSQ_SUMTYPE int
#define SUMSQ_SIDEX(side) (((SUMSQ_SUMTYPE) side) * side)
#define SUMSQ_SIDE2(side) (((int) side) + side)
#define SUMSQ_MAXCELLLINES 4

typedef struct sumsquare_cellrelation_st {
	SUMSQ_NUMTYPE ncelllines, celllines[SUMSQ_MAXCELLLINES];
} sumsquare_cellrelation;

typedef struct sumsquare_linerelation_st {
	SUMSQ_NUMTYPE nlinecells, *linecells;
} sumsquare_linerelation;

typedef struct sumsquare_linecount_st {
	SUMSQ_SUMTYPE sum;
	SUMSQ_NUMTYPE holes;
} sumsquare_linecount;

typedef struct sumsquare_st {
	SUMSQ_NUMTYPE side, nlines, ncells, *nums;
	sumsquare_cellrelation *cellrelations;
	sumsquare_linerelation *linerelations;
	sumsquare_linecount *linecounts;
} *sumsquare;

#define SUMSQ_IFROMPOS(cellidx, side) ((cellidx) / (side))
#define SUMSQ_JFROMPOS(cellidx, side) ((cellidx) % (side))

#define SUMSQUARE_ROWIDX(sq, i) (i)
#define SUMSQUARE_COLIDX(sq, j) (((sq)->side) + (j))
#define SUMSQUARE_DIAGIDX(sq, d) (SUMSQ_SIDE2((sq)->side) + (d))

/** Returns a cellrelation struct with the lines that contain the given cell. */
sumsquare_cellrelation sumsquare_cellrelation_init(int cellidx,
		SUMSQ_NUMTYPE side){
	sumsquare_cellrelation cell;
	int i = SUMSQ_IFROMPOS(cellidx, side);
	int j = SUMSQ_JFROMPOS(cellidx, side);
	int count = 0;
	cell.celllines[count++] = i;
	cell.celllines[count++] = side + j;
	if (i == j) {
		cell.celllines[count++] = SUMSQ_SIDE2(side);
	}
	if (i + j == side - 1) {
		cell.celllines[count++] = SUMSQ_SIDE2(side) + 1;
	}
	cell.ncelllines = count;
	return cell;
}

/** Returns a linerelation struct with the cells contained in the given line.
 * To find them it uses only the function sumsquare_cellrelation_init(). */
sumsquare_linerelation sumsquare_linerelation_init(int lineidx,
					SUMSQ_NUMTYPE side, int ncells,
					sumsquare_cellrelation *cellrelations,
					SUMSQ_NUMTYPE *mainlinecells){
	sumsquare_linerelation line;
	sumsquare_cellrelation cell;
	int c, l, count = 0;
	line.linecells = mainlinecells + (lineidx * side);
	for (c = 0; c < ncells; c++) {
		cell = cellrelations[c];
		for (l = 0; l < cell.ncelllines; l++) {
			if (cell.celllines[l] == lineidx) {
				line.linecells[count++] = c;
				break;
			}
		}
	}
	line.nlinecells = count;
	return line;
}

#define SUMSQUARE_BYTES(side) \
	(sizeof(struct sumsquare_st) \
		+ (SUMSQ_SIDEX(side) * sizeof(SUMSQ_NUMTYPE)) \
		+ (SUMSQ_SIDEX(side) * sizeof(sumsquare_cellrelation)) \
		+ ((SUMSQ_SIDE2(side) + 2) * sizeof(sumsquare_linerelation)) \
		+ ((SUMSQ_SIDE2(side) + 2) * sizeof(sumsquare_linecount)) \
		+ ((SUMSQ_SIDE2(side) + 2) * (side) * sizeof(SUMSQ_NUMTYPE)))

/** Must receive as arguments an array of SUMSQUARE_BYTES(N) bytes and the
 * same number N, and returns the same array initalized as a sumsquare. */
sumsquare sumsquare_init(char *mem, SUMSQ_NUMTYPE side) {
	int c, l;
	SUMSQ_NUMTYPE ncells = SUMSQ_SIDEX(side);
	SUMSQ_NUMTYPE nlines = SUMSQ_SIDE2(side) + 2;
	sumsquare sq = (sumsquare) mem;
	SUMSQ_NUMTYPE *nums = (SUMSQ_NUMTYPE *) (sq + 1);
	sumsquare_cellrelation *cellrelations = (sumsquare_cellrelation *)
					(nums + ncells);
	sumsquare_linerelation *linerelations = (sumsquare_linerelation *)
					(cellrelations + ncells);
	sumsquare_linecount *linecounts = (sumsquare_linecount *)
					(linerelations + nlines);
	SUMSQ_NUMTYPE *mainlinecells = (SUMSQ_NUMTYPE *)
					(linecounts + nlines);
	sq->side = side;
	sq->ncells = ncells;
	sq->nums = nums;
	sq->cellrelations = cellrelations;
	for (c = 0; c < ncells; c++) {
		nums[c] = 0;
		cellrelations[c] = sumsquare_cellrelation_init(c, side);
	}
	sq->nlines = nlines;
	sq->linerelations = linerelations;
	sq->linecounts = linecounts;
	for (l = 0; l < nlines; l++) {
		linerelations[l] = sumsquare_linerelation_init(l, side,
					ncells, cellrelations, mainlinecells);
		linecounts[l].sum = 0;
		linecounts[l].holes = sq->side;
	}
	return sq;
}

#define sumsquare_side(sq) ((sq)->side)
#define sumsquare_nlines(sq) ((sq)->nlines)
#define sumsquare_ncells(sq) ((sq)->ncells)
#define sumsquare_getnum(sq, c) ((sq)->nums[c])
#define sumsquare_getcellrelation(sq, c) ((sq)->cellrelations[c])
#define sumsquare_getlinerelation(sq, l) ((sq)->linerelations[l])
#define sumsquare_getlinecount(sq, l) ((sq)->linecounts[l])

void sumsquare_setnum(sumsquare sq, int cellidx, SUMSQ_NUMTYPE n) {
	SUMSQ_NUMTYPE old = sumsquare_getnum(sq, cellidx);
	int sumdif = n - ((int) old);
	int holesdif = (! old && n) ? -1 : (old && ! n) ? +1 : 0;
	int l;
	sumsquare_cellrelation cell;
	if (sumdif) {
		cell = sumsquare_getcellrelation(sq, cellidx);
		for (l = 0; l < cell.ncelllines; l++) {
			sumsquare_getlinecount(sq, cell.celllines[l]).sum
				+= sumdif;
		}
		sumsquare_getnum(sq, cellidx) += sumdif;
	}
	if (holesdif) {
		if (! sumdif) {
			cell = sumsquare_getcellrelation(sq, cellidx);
		}
		for (l = 0; l < cell.ncelllines; l++) {
			sumsquare_getlinecount(sq, cell.celllines[l]).holes
				+= holesdif;
		}
	}
}

/** Returns the first empty cell of the given line. */
int sumsquare_emptycell(sumsquare sq, int lineidx) {
	sumsquare_linerelation line = sumsquare_getlinerelation(sq, lineidx);
	int c, cellidx;
	for (c = 0; c < line.nlinecells; c++) {
		cellidx = line.linecells[c];
		if (sumsquare_getnum(sq, cellidx) == 0) {
			return cellidx;
		}
	}
	return 0;
}

