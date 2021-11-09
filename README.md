Magic squares generator
=======================

Introduction
------------

C program to generate all NxN magic squares applying some generic optimizations.

A [magic square](https://en.wikipedia.org/wiki/Magic_square) is a matrix of
N rows and N columns containing the numbers 1, 2, 3, ..., NxN distributed in
such a way that the sums of the numbers in every row, column and diagonal are
all equal to the same value, called the magic sum.

The size of the magic squares, the filtering level, the printing style and
enable or disable the direct filling of the lines with one remaining hole
can be configured in this program.

To compile it, in a system with [GCC](https://gcc.gnu.org/) you can run
the following command:

    gcc -O3 -o magicsquare magicsquare.c

Filtered magic squares
----------------------

This program has been used to generate and save in 18 hours a _subset_ of all
the magic squares of size 5x5, where each magic square generated represents
32 variations of it obtained by applying to the square certain transformations
that do not change the numbers of its lines (rows, columns and diagonals).
The total number of magic squares generated and saved is 68826306, that is 1/4
of the [standard counting of magic squares](https://oeis.org/A006052) which
only removes rotations and reflections, and 1/32 of all possible magic squares.
To discard magic squares this program only accepts the squares with
certain relations between the numbers of its diagonals:

1. The number in the top-left corner of the square must be less than the numbers
in the other 3 corners (this removes 3 of 4 squares obtained by rotation):

        1 . . . 2
        . . . . .
        . . . . .
        . . . . .
        3 . . . 4

2. The number in the top-right corner of the square must be less than the number
in the bottom-left corner (this removes 1 of 2 squares not removed with the
previous condition which are obtained by reflecting the square and then
rotating it until having the other two corners in the same place):

        . . . . 1
        . . . . .
        . . . . .
        . . . . .
        2 . . . .

3. The second number in the main diagonal must be less than its opposite number
in the diagonal (this removes 1 of 2 squares not removed with the previous
conditions which are obtained by interchanging the opposite borders and
rotating the square two times to have the four corners in the same place):

        . . . . .
        . 1 . . .
        . . . . .
        . . . 2 .
        . . . . .

4. The number in the top-left corner must be less than the numbers in the
corners top-left, top-right and bottom-left of the 3x3 central square (this
removes 1 of 2 squares not removed in the previous conditions which are obtained
by interchanging each border of the square with its adjacent row or column):

        1 . . . .
        . 2 . 3 .
        . . . . .
        . 4 . . .
        . . . . .

All the 3x3 and 4x4 magic squares are generated in seconds, filtered or not.

Output format
-------------

Selecting the print style 0, the program counts and prints the number of magic
squares generated, and it can also print the squares in table format (4), but
to save space it also has a reduced _short_ format (1) that removes the spaces,
uses letters instead of numbers with two digits and removes the numbers of the
last column, the last row and the second number of the row previous to the last,
since these numbers can be calculated. For example, the first magic square
generated for 5x5 is 1IKON86CJ3P7H49, that must be interpreted as:

     1 18 20 24  .
    23  8  6 12  .
    19  3 25  7  .
    17  .  4  9  .
     .  .  .  .  .

And filling the missing numbers to get the magic sum for 5x5 (65) we get:

     1 18 20 24  2
    23  8  6 12 16
    19  3 25  7 11
    17 21  4  9 14
     5 15 10 13 22

This is written 1IKO2N86CGJ3P7BHL49E5FADM in the reduced _long_ format (2),
and the last format provided is the one-line decimal format (3):

    1,18,20,24,2,23,8,6,12,16,19,3,25,7,11,17,21,4,9,14,5,15,10,13,22

Technical details
-----------------

This program uses a backtracking algorithm with some optimizations:

- Storing the used and unused numbers and positions in a special data structure
based on a double-linked list plus a stack for undoing the changes.
- Maintaining the sum and number of holes of each line (row, column, diagonal)
and calculating their mininum and maximum sums after changing a number as a way
to check if the magic square remains possible.
- Discarding many magic squares that can be generated transforming others.
- Filling first the positions in the diagonals because they allow to discard
early the unwanted squares.
- Adding the numbers directly when any line has only one hole unfilled.

The data structures used in this program are defined in a way that allows to
create them using static or dynamic memory, offering a preprocessor macro that
returns the memory in bytes needed by the structure as a function of the
required size, and also a function to initialize that memory before use it.

