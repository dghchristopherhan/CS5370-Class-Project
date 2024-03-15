/****************************************************************************
*
*	calc.h:	Implementation of calculations: average, sort, median, confidence 
*           intervals, thresholds for highest/lowest percentiles
*
*                       Sharlee Climer
*                       January 2003
*
****************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <string.h>


const int PRINT_HIST = 0; // set to 1 to print histogram
const int NUM_BINS = 1000; // number of bins for histogram

void fatal(const char*);

float calcAvg(float * values, int size);  // calculate average of values
void sortArray(float *data, int size);
void printStatistics(double *data, int size, char *outputFile);




