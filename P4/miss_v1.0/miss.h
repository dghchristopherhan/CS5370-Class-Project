// -------------------------------------------------------------------------
// miss.h -   Header file for program to remove missing data
//
// written by Sharlee Climer, December 2011
//
// ------------------------------------------------------------------------

#ifndef _MISS_H
#define _MISS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <cstdlib>

#include "timer.h"


const int SNP_DATA = 0; // if 1, will check for space between alleles, treat '0' as missing data

const int CUSTOM_MISSING = 0; // set to 1 to have a custom missing symbol
const int MISSING_SYMBOL = 0; // ASCII value of customized symbol for missing data
// 'Y' is 89, '-' is 45

const int LOG_FILE = 1; // set to 1 to record screen output to log file (Boolean)

const int QUIET = 1;  // set to one to eliminate output to screen (Boolean)
const int VERBOSE = 0;  // set to one to display maximum output to screen (Boolean)

const int NUM_CHARS_STRNG = 500; // number of chars used for storage of strings
const int NUM_MAX_CHARS = 2000; // maximum number of chars in any input string
const int MAX_NUM_ROWS = 10000000; // maximum number of rows
const int MAX_NUM_COLS = 10000000; // maximum number of columns
const int MAXNUMHEADERS = 200; // maximum number of header rows or columns

const double TOL = 0.00001; // tolerance

inline void warning(const char* p) { fprintf(stderr,"Warning: %s \n\n",p); }
inline void fatal(const char* string) {fprintf(stderr,"\nFatal: %s\n\n",string); exit(1); }

#endif
