/****************************************************************************
*
*	calc.cpp:	Implementation of calculations: average, sort, median, confidence 
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


const int PRINT_HIST = 1; // set to 1 to print histogram
const int NUM_BINS = 1000; // number of bins for histogram

void fatal(const char*);

float calcAvg(float * values, int size)  // calculate average of values
{
  float avg = 0;
  for (int i = 0; i < size; i++)
    avg += values[i];
  return (avg / (float)size);
}

void sortArray(float *data, int size)
{
  if (size > 1) 
    std::sort(data, data + size);
}

void printStatistics(double *data, int size, char *outputFile)
{
  double avgdata = 0.0;
  double sdata = 0.0;
  double confUpData, confLowData;
  double tempData;
  double median;
 
  if (size > 1) {
    std::sort(data, data + size); // sort data using quicksort

    if ((size % 2) == 0)  // an even number of data
      median = 0.5 * (data[size/2] + data[size/2 - 1]);
    
    else        // odd number of data
      median = data[size/2];

    for (int i = 0; i < size; i++)     // find mean of data
      avgdata += data[i];
    avgdata /= (double)size;

    for (int i = 0; i < size; i++)  // find 95% confidence interval
      sdata += (data[i] - avgdata) * (data[i] - avgdata);
    sdata /= (double)((size - 1) * size);
    sdata = sqrt(sdata);
  }

  else {
    median = avgdata = data[0];
    sdata = 0;
  }

  confUpData = avgdata + (2.0 * sdata);
  confLowData = avgdata - (2.0 * sdata);

  FILE *f1;
  
  // check for file with same name already in directory
  if ((f1 = fopen(outputFile, "r")) != NULL) 
    fatal("File with same name already exists in directory.");
  
  f1 = fopen(outputFile, "w");                    // output the results
  fprintf(f1, "max = %0.5f, min = %.05f \nmedian = %0.5f \naverage = %0.5f \n95confup = %0.5f, 95confdown = %0.5f \n\n", data[size-1], data[0], median, avgdata,
           confUpData, confLowData);

  double *histThresh; // hold thresholds for each bin for histogram
  int *hist; // hold number of values in each bin
 
  if(PRINT_HIST) { // compute values for histogram

    // allocate memory
    if((histThresh = new double[NUM_BINS]) ==NULL)
      fatal("memory not allocated");

    if((hist = new int[NUM_BINS]) ==NULL)
      fatal("memory not allocated");

    // initialize bin counts
    for (int i = 0; i < NUM_BINS; i++)
      hist[i] = 0;

    // determine increment size for bins
    double incr = (data[size-1] - data[0]) / NUM_BINS;

    histThresh[0] = data[0] + incr; // first bin's max value

    for (int i = 1; i < NUM_BINS; i++)
      histThresh[i] = histThresh[i-1] + incr;

    int threshPtr = 0; // pointer for bin threshold
    int dataPtr = 0; // pointer for data

    while (dataPtr < size) {
      int count = 0; // count number in each bin

      while((data[dataPtr] <= histThresh[threshPtr]) && (dataPtr < size)) {
	count++; // count number of values in bin
	dataPtr++; // move through sorted data
      }

	hist[threshPtr++] = count; // record number of values in bin
    }

    fprintf(f1, "Histogram:\nMax Bin Value\tNumber in Bin\n");

    for (int i = 0; i < NUM_BINS; i++)
      fprintf(f1, "%f\t%d\n", histThresh[i], hist[i]);

    delete [] hist;
    delete [] histThresh;
  }

  fclose(f1);
}




