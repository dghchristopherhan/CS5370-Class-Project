// -------------------------------------------------------------------------
// discretize.h
//
// written by Sharlee Climer, August 2022
//
// ------------------------------------------------------------------------

#ifndef _DISCRETIZE_H
#define _DISCRETIZE_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include "timer.h"

const int NUM_HEAD_COLS = 1;// number of header columns in data file
const int NUM_HEAD_ROWS = 1; // number of header rows in data file
const int NUM_END_FILE = 0; // number of words at end of input file

const int STRNG_LENGTH = 200; // length of string array
const float MISSING = -100000.0; // mark as missing value
const float MAX = 10000.0; // maximum data value can be
const float MIN = -10000.0; // minimum data value can be (needs to be more than MISSING value)

const int QUIET = 0;  // set to one to reduce output to screen
const int VERBOSE = 0;  // set to one to display maximum output to screen
const int MAX_NUM_INDIVIDUALS = 100000; // maximum number of individuals
const int MAX_NUM_SNPS = 10000000; // maximum number of SNPs
const float TOL = 0.000001; // tolerance

inline void warning(const char* p) { fprintf(stderr,"Warning: %s \n",p); }
inline void fatal(const char* string) {fprintf(stderr,"\nFatal: %s\n\n",string); exit(1); }

#endif
