/*   
 *
 *   discretize.cpp 
 *
 *   Program for discretizing data to low=-1, neutral=0, high=1
 *    
 *
 *   Written by Sharlee Climer
 *   August 2022
 *
 *
*/

#include "discretize.h"
#include "calc.h"

void thresh(float *, int, float, int, float *, float *);

int main(int argc, char ** argv)
{
  if (argc != 6)
    fatal("Usage:\n  discretize input.gex numGenes numInd percent output.txt"); 

  FILE *input; // contains gene expression data
  FILE *output;

  timer t;

  t.start("\nTimer started");  

  std::cout << "Command line arguments: \n\t";
  for (int i = 0; i < argc; i++)
	std::cout << argv[i] << " ";
  std::cout << "\n" << std::endl;

  // open input file
  if ((input = fopen(argv[1], "r")) == NULL)
    fatal("Input file could not be opened");

  int numGenes = atoi(argv[2]);  // number of genes  
  int numInd = atoi(argv[3]); // number of individuals
  float percent = atof(argv[4]); // percentage of values above/below threshold

  float threshLow; // hold thresholds for low and high expression
  float threshHi;
  char strng[STRNG_LENGTH]; // string to hold information temporarily
  long int numMissing = 0; // number of missing data values

  if (!QUIET) {
    std::cout << "Discretizing data for " << numGenes << " genes and " << numInd << " individuals." << std::endl;

    std::cout << "Assuming there are " << NUM_HEAD_ROWS << " header row(s) and " << NUM_HEAD_COLS << " header column(s).\nAssuming there are " << NUM_END_FILE << " word(s) at end of file.\n" << std::endl;
  }

  if ((percent < 0.0 + TOL) || (percent > 0.5 - TOL))
    fatal("Invalid percentage. Must be between 0 and 0.5.");

  if((numGenes < 1) || (numInd < 1))
    fatal("Too few genes or individuals");

  // allocate memory into an array for holding all expression values
  float *data;
  if((data = new float[numGenes*numInd]) ==NULL)
    fatal("memory not allocated");

  // read in header information
  int numCols = NUM_HEAD_COLS + numInd; 
  for (int i = 0; i < NUM_HEAD_ROWS; i++)
    for (int j = 0; j < numCols; j++) 
      fscanf(input, "%s", strng); // not using header info
  
  // read in and record data
  for (int i = 0; i < numGenes; i++) {
    for (int j = 0; j < NUM_HEAD_COLS; j++) {
      if (feof(input))
	      fatal("Missing data in input file");
      fscanf(input, "%s", strng); // not using header info
    }
    
    for (int j = 0; j < numInd; j++) {
      int missing = 0; // set to 1 if missing data
      if (feof(input))
	      fatal("Missing data in input file"); 
      fscanf(input, "%s", strng);

      // check for valid number
      
      if((strng[0] < '0') || (strng[0] > '9')) {
        if((strng[0] != '-') && (strng[0] != '.')) {
          missing = 1;
          numMissing++;
          data[i* numInd + j] = MISSING;
        }
      }
      
      if (!missing) {
        data[i* numInd + j] = atof(strng);
        if ((data[i* numInd + j] < MIN + TOL) || (data[i* numInd + j] > MAX)) {
          std::cout << data[i* numInd + j] << std::endl;
	        fatal("Invalid data value. Adjust MIN and MAX in header file if needed."); 
        }
      }
    }
  }

  if (VERBOSE) {
    std::cout << "Data read in:" << std::endl;
    for (int i = 0; i < numGenes; i++) {
      for (int j = 0; j < numInd; j++)
	      std::cout << data[i*numInd + j] << " ";
      std::cout << std::endl;
    }
  }

  for (int i = 0; i < NUM_END_FILE; i++) {
    if (feof(input))
      fatal("Missing words at end of input file.  Check header file.");

    fscanf(input, "%s", strng); // read in strings at end of file
  }

  // check for end of file
  if(!feof(input)) {
	fscanf(input, "%s", strng);
	if (!feof(input))
	  fatal("Unread information in input file");
  }

  fclose(input);
  std::cout << numMissing << " missing values.\n" << std::endl;
  std::cout << "Computing thresholds that correspond to top and bottom " << percent * 100 << "% of values.\n" << std::endl;

  thresh(data, numGenes * numInd, percent, numMissing, &threshLow, &threshHi);

  // write out discretized data
  if ((input = fopen(argv[1], "r")) == NULL)
    fatal("Input file could not be opened");

  if ((output = fopen(argv[5], "w")) == NULL)
    fatal("Output file could not be opened");

  // read in and write out header information
  for (int i = 0; i < NUM_HEAD_ROWS; i++) {
    for (int j = 0; j < numCols; j++) {
      fscanf(input, "%s", strng); 
      fprintf(output, "%s\t", strng);
    }

    fprintf(output, "\n");    
  }
  
  // read in and write out discretized values
  for (int i = 0; i < numGenes; i++) {
    for (int j = 0; j < NUM_HEAD_COLS; j++) {
      fscanf(input, "%s", strng); 
      fprintf(output, "%s\t", strng);      
    }
    
    for (int j = 0; j < numInd; j++) {
      fscanf(input, "%s", strng); 
      float val = atof(strng);
      int printVal = 0; // value to be printed out
       
      if (val >= threshHi - TOL) 
        printVal = 1; 
        
      if (val <= threshLow + TOL) 
        printVal = -1;
        
      if((strng[0] < '0') || (strng[0] > '9')) 
        if((strng[0] != '-') && (strng[0] != '.')) 
          printVal = -2;           // missing data
        
      fprintf(output, "%d\t", printVal); 
    }
    
    fprintf(output, "\n");
  }
  
  fclose(input);
  fclose(output);
  
  if (!QUIET)
    std::cout << "Discretized data written to '" << argv[5] << "', where 1 = high, 0 = neutral, and -1 = low expression. \nMissing data is indicated by -2.\n\n";

  t.stop("Timer stopped");
  std::cout << t << " seconds\n" << std::endl;

  return 1;
}




//compute threshold corresponding to given percent of values
void thresh(float *data, int size, float percent, int numMissing, float *threshLow, float *threshHi)
{
  double avgdata = 0.0;
  double median;
  int numNoMissing = size - numMissing; // number of actual data values
  
  if (size < 1) 
    fatal("Invalid array size.");
 
  if (size > 1) {
    std::sort(data, data + size); // sort data using quicksort

    if (((numNoMissing) % 2) == 0)  // an even number of data
      median = 0.5 * (data[numMissing + numNoMissing/2] + data[numMissing + numNoMissing/2 - 1]);
    
    else        // odd number of data
      median = data[numMissing + numNoMissing/2];

    for (int i = numMissing; i < size; i++)     // find mean of data
      avgdata += data[i];
    avgdata /= (double)numNoMissing;

  }

  else 
    median = avgdata = data[0];

  if (VERBOSE) {
    std::cout << "Sorted data:" << std::endl;
    for (int i = 0; i < size; i++)
      std::cout << data[i] << " ";
    std::cout << "\n" << std::endl;
  }

  if(!QUIET)
    std::cout << "Average value = " << avgdata << ", and median = " << median << std::endl;

  //determine number of values above/below threshold
  float temp = (float)numNoMissing * percent;
  int num = (int)ceil(temp); // round up 

  if(!QUIET)
    std::cout << "At least " << num << " values above and below thresholds.\n" << std::endl;

  if (num < 1)
    fatal("Must have at least one value above or below threshold");

  *threshLow = data[numMissing + num-1]; // lower threshold
  *threshHi = data[size-num];  // upper threshold

  std::cout << "Upper threshold = " << *threshHi << std::endl;
  std::cout << "Lower threshold = " << *threshLow << "\n" << std::endl;
  
  if (*threshHi < *threshLow)
    fatal("Thresholds overlap.");

  if(!QUIET){
    // count number of ties
    int ties = 0; // number of ties
    
    for (int i = 0; i < size; i++)
      if(data[i] >= *threshHi - TOL) // allow for round-off error
        ties++;
    
    std::cout << ties - num << " values are added due to ties for upper threshold." << std::endl;
    
    ties = 0; // number of ties
    
    for (int i = numMissing; i < size; i++)
      if(data[i] <= *threshLow + TOL) // allow for round-off error
	      ties++;
    
    std::cout << ties - num << " values are added due to ties for lower threshold.\n" << std::endl;
  }
}


// convert number from scientific notation to float format
void convertSciNotation(char *strng) {
  char tempStrng[STRNG_LENGTH]; // hold old string 

  // check for decimal point in expected position
  if(strng[1] != '.') {
    std::cout << strng << std::endl;
    fatal("Data value has unexpected format.");
  }

  for (int i = 0; i < STRNG_LENGTH; i++)
    tempStrng[i] = strng[i];

  int numSig = 0; // count number of significant digits
  while ((strng[numSig] != 'E') && (strng[numSig] != 'e')) {
    numSig ++; 
    if (numSig == STRNG_LENGTH) {
      std::cout << strng << std::endl;
      fatal("Invalid value in data file");
    }
  }

  // check number of significant digits
  if (numSig < 3) {
    std::cout << strng << std::endl;
    fatal("Unexpected value format in data file");
  }

  // check for negative exponent
  if (strng[numSig + 1] != '-') {
    std::cout << strng << std::endl;
    fatal("Invalid value in data file");
  }

  // determine exponential value
  int exp = 0; // exponent
  for (int i = 0; i < STRNG_LENGTH-numSig-1; i++) {
    exp += strng[numSig + i + 2] - 48; // add next digit (convert ASCII)

    if (strng[numSig + i + 3] != '\0') // not to end of number yet
      exp *= 10; // compute base 10 number
    else
      break; // reached end of number
  }

  // check exponent
  if (exp < 1) {
    std::cout << strng << std::endl;
    fatal("Unexpected data format");
  }

  // write new number in strng
  strng[0] = '0'; // set initial zero
  strng[1] = '.'; // add decimal point

  for (int i = 1; i < exp; i++)
    strng[1+i] = '0'; // add appropriate number of leading zeros

  strng[exp+1] = tempStrng[0]; // add first significant digit

  for (int i = 1; i < numSig-1; i++)
    strng[exp+i+1] = tempStrng[i+1]; // add on other significant digits

  strng[exp+numSig] = '\0'; // end of string
  
  if (VERBOSE) 
    std::cout << "Converted to " << strng << ".\n" << std::endl;
}

