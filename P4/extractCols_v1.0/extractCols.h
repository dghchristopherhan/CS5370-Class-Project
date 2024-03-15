// -------------------------------------------------------------------------
// extractCols.h
//
// written by Sharlee Climer, December 2011
//
// ------------------------------------------------------------------------

#ifndef _EXTRACTCOL_H
#define _EXTRACTCOL_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string.h>

#include "timer.h"


const int NUMHEADROWS = 1; // number of header rows in input file
const int NUMHEADCOLS = 1; // number of header columns in input file

const int WRITEHEADCOLS = 1; // set to 1 to include header columns in output

const int WRITE_REMAINDER = 0; // set to 1 to have remainder columns written to 'remainder.txt'

const int QUIET = 1;  // set to one to eliminate output to screen
const int VERBOSE = 0;  // set to one to display maximum output to screen
const int MAX_NUM_INDIVIDUALS = 100000; // maximum number of individuals
const int MAX_NUM_SNPs = 10000000; // maximum number of SNPs

const float TOL = 0.000001; // tolerance

inline void warning(const char* p) { fprintf(stderr,"Warning: %s \n",p); }
inline void fatal(const char* string) {fprintf(stderr,"\nFatal: %s\n\n",string); exit(1); }

#endif
