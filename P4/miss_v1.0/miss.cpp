/****************************************************************************
*
*      miss.cpp:	Code for cleaning out variables with too much missing
*                         data.
*      
*
*                       Sharlee Climer
*                       December 2011
*                       Revised to run iteratively and produce logfile
*                       May 2021
*
*                       
****************************************************************************/
  

#include "miss.h"


void checkConstants(); // check validity of constants in miss.h


int main(int argc, char ** argv)
{
  if (argc != 6)
    fatal("Usage:\n\n   miss input.txt numDataRows numDataColumns numHeaderRows numHeaderCols\n\n"); 

  timer t;
  t.start("Timer started.");

  std::cout << "\nCommand line arguments: \n\t";
  for (int i = 0; i < argc; i++)
	std::cout << argv[i] << " ";
  std::cout << "\n" << std::endl;

  std::cout << "\nAny value starting with characters 'N', '?', 'X'";
  if(CUSTOM_MISSING)
    std::cout << ", '" << (char)MISSING_SYMBOL << "', ";
  if(SNP_DATA)
    std::cout << ", or zero will be considered missing data."<< std::endl;
  else
    std::cout << " will be considered missing data."<< std::endl;

  checkConstants(); // check validity of constants defined in miss.h

  // determine format of data
  int space = 0; // set to 1 if there are spaces between the values
  int format = 0; // set to 1 when format is determined
  char strng[NUM_MAX_CHARS]; // temporary string storage

  if(SNP_DATA) { // SNP data may have a space between the allele pair 
    std::cout << "Does each genotype have a space between its two alleles? (y or n): ";
    std::cin >> strng;

    if((strncmp(strng, "Y", 1) == 0) || (strncmp(strng, "y", 1) == 0)) {
      space = 1; // expect a space between pair of alleles for each genotype
      format = 1; // format is determined
    }

    if((strncmp(strng, "N", 1) == 0) || (strncmp(strng, "n", 1) == 0)) {
      space = 0; // expect no space between pair of alleles for each genotype
      format = 1; // format is determined
    }

    // remove fatal() and add a loop later
    if(format == 0) { //input not recognized
      fatal("Expected 'Y', 'y', 'N', or 'n' for first (or only) letter. Please try again.");
    }
  }

  if (!SNP_DATA)
    format = 1; 

  FILE *input; // input data file
  FILE *output; // output data file
  FILE *logfile; // log file that records screen output
  FILE *newFile; // temporary file 

  char logfileName[200]; // hold name of logfile
  char outputFileName[200]; // hold name of output file
  char base[100]; // base for logfile name
  const char logSuffix[] = ".miss.log"; // suffix for logfile name
  const char outSuffix[] = ".cleaned.tsv"; // suffix for output file name
  
  // determine name for logfile and output file
  for (int i = 0; i < 100; i++) {
    base[i] = argv[1][i]; // copy input file name
    if (base[i] == '\0') {
      if(i < 5)
	fatal("Expected output file name to have a 3-character suffix");
      
      base[i-4] = '\0'; // set end of string to not include 3-character suffix
      break;
    }
  }
  
  sprintf(logfileName, "%s%s", base, logSuffix); // string together file name
  sprintf(outputFileName, "%s%s", base, outSuffix); 
  
  // check to see if logfile already exists, and append to it if it does (with a warning message)
  if(LOG_FILE) 
    std::cout << "Screen output will be recorded in '" << logfileName <<"'.\n" << std::endl;

  if ((logfile = fopen(logfileName, "r")) != NULL) {
    warning("Log file already exists, will append new output.");
    fclose(logfile); 
  }

  else 
    if ((logfile = fopen(logfileName, "w")) == NULL) {
      fatal("Log file could not be opened.\n");
      fclose(logfile);
    }

  if ((logfile = fopen(logfileName, "a")) == NULL)
      fatal("Log file could not be accessed.\n");

  if (LOG_FILE) {
    fprintf(logfile, "\nCommand line arguments: \n\t");
    for (int i = 0; i < argc; i++)
      fprintf(logfile, "%s ", argv[i]);
    fprintf(logfile, "\n\n");
  }

  int numRows = atoi(argv[2]); // number of rows of data
  int numCols = atoi(argv[3]);  // number of columns of data

  int numheadrows = atoi(argv[4]); // number of header rows and columns
  int numheadcols = atoi(argv[5]);

  std::cout << numRows << " rows and " << numCols << " columns of data, plus ";
  std::cout << numheadrows << " header rows and " << numheadcols << " header columns." << std::endl;

  if(LOG_FILE) {
    fprintf(logfile, "%d rows and %d columns of data, plus %d header rows and %d header columns.\n", numRows, numCols, numheadrows, numheadcols);

    if(SNP_DATA) {
      if(space)
	fprintf(logfile, "Assumed there is a space separating nucleotides for each genotype.\n");
      
      else
	fprintf(logfile, "Assumed there is no space separating nucleotides for each genotypes.\n");
    }
  }

  // check validity of numbers of rows and columns
  if (numRows > MAX_NUM_ROWS)
	fatal("Too many rows.  Fix header file.");
  if (numCols > MAX_NUM_COLS)
	fatal("Too many columns.  Fix header file.");
  if ((numRows < 1) || (numCols < 1))
	fatal("Too few rows or columns");

  if (numheadrows > MAXNUMHEADERS)
	fatal("Too many rows.  Fix header file.");
  if (numheadcols > MAXNUMHEADERS)
	fatal("Too many columns.  Fix header file.");
  if ((numheadrows < 0) || (numheadcols < 0))
	fatal("Too few rows or columns");
  
  // allocate missing numbers arrays memory
  float *rowsMiss; // hold number of missing values for each row
  float *colsMiss; // hold number of missing values for each column

  if ((rowsMiss = new float[numRows]) == NULL)
    fatal("memory not allocated");

  if ((colsMiss = new float[numCols]) == NULL)
    fatal("memory not allocated");

  // hold number of missing values for each threshold
  int *threshRows; 
  int *threshCols; 
  float increment; 

  if ((threshRows = new int[101]) == NULL)
    fatal("memory not allocated");

  if ((threshCols = new int[101]) == NULL)
    fatal("memory not allocated"); 

  int keepLooping = 1; // change to zero when finished removing rows/columns

  char inputFileName[NUM_MAX_CHARS]; // hold current input file name
  
  for (int i = 0; i < NUM_MAX_CHARS; i++) { // copy in first input file name
    inputFileName[i] = argv[1][i]; 
    if (argv[1][i] == '\0')
      break;
  }
  
    
  while (keepLooping) { // continue until user stops iterations
  
  if ((input = fopen(inputFileName, "r")) == NULL)
    fatal("Input file could not be opened.\n");
    
  // initialize values to zero
  for (int i = 0; i < numRows; i++)
    rowsMiss[i] = 0;

  for (int i = 0; i < numCols; i++)
    colsMiss[i] = 0;

  // read in header rows and disregard
  for (int i = 0; i < numheadrows; i++)
    for (int j = 0; j < numheadcols + numCols; j++)
      fscanf(input, "%s", strng);

  // read in data

  int ascii1; // hold ascii values of first allele

  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numheadcols; j++) 
      fscanf(input, "%s", strng); // read in and disregard header columns

    for (int j = 0; j < numCols; j++) {
      if(feof(input))
	      if((i != numRows) || (j != numCols)) // might not have any whitespace after last entry
          fatal("Missing data in input file.");

      fscanf(input, "%s", strng);
      //std::cout << i << ", " << j << ": " << strng << std::endl;

      ascii1 = strng[0]; // find ascii value of first char

      if (space) 
	fscanf(input, "%s", strng); // read in second allele
 
      // increase tallies if missing value

      // allow any value for ascii2 when ascii1 is 'N' ('NA', 'NN', etc.)
      if (ascii1 == 78) { // 'N'
	rowsMiss[i]++; // one more missing in this row
	colsMiss[j]++; // one more missing in this column
	continue;
      }

      if(ascii1 == MISSING_SYMBOL) { // customized missing symbol
	rowsMiss[i]++; // one more missing in this row
	colsMiss[j]++; // one more missing in this column
	continue;
      }

      if((ascii1 == 48) && (SNP_DATA == 1)) { // '0'
	rowsMiss[i]++; // one more missing in this row
	colsMiss[j]++; // one more missing in this column
	continue;
      }

      if ((ascii1 == 63) || (ascii1 == 88)) { // '?' or 'X'
	rowsMiss[i]++; // one more missing in this row
	colsMiss[j]++; // one more missing in this column
	continue;
      }
    }
  }

  
  // check for end of file
  if (!feof(input)){
    fscanf(input, "%s", strng);
    if (!feof(input))
      fatal("Unread data in input file");
  }

  fclose(input);
  
  if (VERBOSE) {
    std::cout << "\nNumber of missing values for each row:\n" << std::endl;

    for (int i = 0; i < numRows; i++)
      std::cout << " Row " << i+1 << ": " << rowsMiss[i] << " missing values" << std::endl;

    std::cout << std::endl;

    std::cout << "Number of missing values for each column:\n" << std::endl;

    for (int i = 0; i < numCols; i++)
      std::cout << " Column " << i+1 << ": " << colsMiss[i] << " missing values" << std::endl;

    std::cout << std::endl;
  }

  // determine percentages of missing values
  for (int i = 0; i < numRows; i++)
    rowsMiss[i] /= (float)numCols;

  for (int i = 0; i < numCols; i++)
    colsMiss[i] /= (float)numRows;

  if (VERBOSE) {
    std::cout << "\nPercentage of missing values for each row:\n" << std::endl;

    for (int i = 0; i < numRows; i++)
      std::cout << " Row " << i+1 << ": " << rowsMiss[i] << " missing" << std::endl;

    std::cout << std::endl;

    std::cout << "Percentage of missing values for each column:\n" << std::endl;

    for (int i = 0; i < numCols; i++)
      std::cout << " Column " << i+1 << ": " << colsMiss[i] << " missing" << std::endl;

    std::cout << std::endl;
  }

  // check validity of values
  for (int i = 0; i < numRows; i++)
    if ((rowsMiss[i] < 0 - TOL) || (rowsMiss[i] > 1 + TOL))
      fatal("Error in calculations of percentages");

  for (int i = 0; i < numCols; i++)
    if ((colsMiss[i] < 0 - TOL) || (colsMiss[i] > 1 + TOL))
      fatal("Error in calculations of percentages");

  // determine number of rows (columns) within each threshold of % missing


  // initialize values to zero
  for (int i = 0; i < 101; i++)
    threshRows[i] = threshCols[i] = 0;

  // tally numbers for each threshold

  for (int i = 0; i < numRows; i++) 
    for (int j = 0; j <= 100; j++) {
      increment = (float)j / 100.0;
      if (rowsMiss[i] < increment + TOL)
        threshRows[j]++; // tally of those with x% missing
    }

  for (int i = 0; i < numCols; i++) 
    for (int j = 0; j <= 100; j++) {
      increment = (float)j / 100.0;
      if (colsMiss[i] < increment + TOL) 
        threshCols[j]++; // tally of those with 0% missing
    }

  // output results to screen
  std::cout << "\n\t\tNumber of Rows\t  Number of Columns" << std::endl;
  for(int i = 0; i <= 50; i++)
	  std::cout << " Allow " << i << "% missing:\t" << threshRows[i] << "\t\t" << threshCols[i] << std::endl;
  
  for(int i = 51; i <= 100; i++)
  {  if(VERBOSE) 
	     std::cout << " Allow " << i << "% missing:\t" << threshRows[i] << "\t\t" << threshCols[i] << std::endl;
           
     if(!VERBOSE && (i % 10 == 0))  // only print out every 10 for higher values
	     std::cout << " Allow " << i << "% missing:\t" << threshRows[i] << "\t\t" << threshCols[i] << std::endl;
  }
  
    std::cout << std::endl;

  // check 100% values
  if ((threshRows[100] != numRows) || (threshCols[100] != numCols))
    fatal("Allowance of 100% is not correct");

  std::cout << "Would you like to delete some of the rows or columns? (y or n):  ";

  std::cin >> strng;
  int writeFile = -1;
  
  if((strncmp(strng, "Y", 1) == 0) || (strncmp(strng, "y", 1) == 0)) 
    writeFile = 1; // write out new file
  
  if((strncmp(strng, "N", 1) == 0) || (strncmp(strng, "n", 1) == 0)) {
    writeFile = 0; // don't write out new file
    break; // break out of keepLooping loop
  }
  
  while(writeFile == -1) { //input not recognized
    std::cout << "\nExpected 'Y', 'y', 'N', or 'n' for first (or only) letter. Please try again.\n\n";
    std::cin >> strng;
  
    if((strncmp(strng, "Y", 1) == 0) || (strncmp(strng, "y", 1) == 0)) 
      writeFile = 1; // write out new file
  
  if((strncmp(strng, "N", 1) == 0) || (strncmp(strng, "n", 1) == 0)) 
    writeFile = 0; // don't write out new file
  }
  
  if (writeFile == 0)  // done with iterations
    break; // break out of keepLooping loop

      std::cout << "Max % missing for rows (0 to 100): ";
      std::cin >> strng; 
      float maxRow = atof(strng);

      std::cout << "Max % missing for columns (0 to 100): ";
      std::cin >> strng; 
      float maxCol = atof(strng);

      // check validity of input values
      if((maxRow > 100.0 + TOL) || (maxRow < 0.0 - TOL))
	fatal("Invalid maximum % missing rows.  Expected value between 0 and 100");
      if((maxCol > 100.0 + TOL) || (maxCol < 0.0 - TOL))
	fatal("Invalid maximum % missing columns.  Expected value between 0 and 100");

      if(LOG_FILE) 
	      fprintf(logfile, "\nMaximum of %.0f%% and %.0f%% missing in rows and columns, respectively.\n", maxRow, maxCol);


      // convert to fractional value (for comparisons)
      maxRow /= 100.0;
      maxCol /= 100.0;
      
      // open data file
      if ((input = fopen(inputFileName, "r")) == NULL)
	fatal("Input file could not be opened.\n");
 
      if ((output = fopen(outputFileName, "w")) == NULL)
        fatal("Output file could not be opened.\n");
 
      // read in and write out header rows
      for (int i = 0; i < numheadrows; i++) {
	for (int j = 0; j < numheadcols; j++) {
	  fscanf(input, "%s", strng);
	  fprintf(output,"%s\t",strng);  
	  //std::cout << strng << std::endl;
	}

	for (int j = 0; j < numCols; j++) {
	  fscanf(input, "%s", strng);
	  if (colsMiss[j] < maxCol+TOL) {
	    fprintf(output, "%s\t", strng); // only print header columns for columns to keep
    }  
	}

	fprintf(output,"\n");
      }

      // read in data, and write out appropriate data
      
      for (int i = 0; i < numRows; i++) {
	for (int j = 0; j < numheadcols; j++) {
	  fscanf(input, "%s", strng); // read in header columns
	  if (rowsMiss[i] < maxRow+TOL) {
	    fprintf(output, "%s\t", strng); // don't print header if won't keep row
    }
	}
	
	for (int j = 0; j < numCols; j++) {
	  fscanf(input, "%s", strng);     
	  if ((rowsMiss[i] < maxRow+TOL) && (colsMiss[j] < maxCol+TOL)) {
	    fprintf(output, "%s\t", strng);
    }     

	  if (space) { // read in and write out second string
	    fscanf(input, "%s", strng);     
	    if ((rowsMiss[i] < maxRow+TOL) && (colsMiss[j] < maxCol+TOL)) {
	      fprintf(output, "%s\t", strng);   
      }
	  }
	}
	
	if (rowsMiss[i] < maxRow+TOL) {
	  fprintf(output,"\n");
    }
      }
   
      // determine new numbers of rows and columns and write to log file

      if(LOG_FILE) 
	fprintf(logfile, "\nOmitted rows:\n");
	
      int newNumRows = 0; // new number of data rows
      int newNumCols = 0; // new number of data columns
      
      for (int i = 0; i < numRows; i++) {
	if (rowsMiss[i] < maxRow+TOL)
	  newNumRows++; // one more row kept
	else
	  if(LOG_FILE) 
	    fprintf(logfile, "%d ", i+1); // print out deleted row number
      }

      if (LOG_FILE)
	fprintf(logfile, "\n\nOmitted columns:\n");

      for (int i = 0; i < numCols; i++) {
	if (colsMiss[i] < maxCol+TOL)
	  newNumCols++; // one more row kept
	else
	  if(LOG_FILE) 
	    fprintf(logfile, "%d ", i+1); // print out deleted column number
      }

      fclose(input);
      fclose(output);
      
      // read in output file and copy to temporary file
      
      if ((newFile = fopen("tempFile", "w")) == NULL) 
	      fatal("Temporary file could not be opened");     

      if ((output = fopen(outputFileName, "r")) == NULL)
        fatal("Temporary file could not be opened.\n");

      // read in and write out header rows
      for (int i = 0; i < numheadrows; i++) {
	      for (int j = 0; j < numheadcols; j++) {
	        fscanf(output, "%s", strng);
          if(feof(output))
            fatal("Output file is missing data");
	      fprintf(newFile,"%s\t",strng);  
	      }

	      for (int j = 0; j < newNumCols-1; j++) {
	        fscanf(output, "%s", strng);
            if(feof(output))
              fatal("Output file is missing data");
	        fprintf(newFile, "%s\t", strng); 
	      }

        fscanf(output, "%s", strng);
          if(feof(output))
            fatal("Output file is missing data");
	      fprintf(newFile,"%s\n", strng);
      } 
      
      // read in and write out remaining rows       
      for (int i = 0; i < newNumRows; i++) {
	      for (int j = 0; j < numheadcols; j++) {
	        fscanf(output, "%s", strng); // read in header columns
            if(feof(output))
              fatal("Output file is missing data");
          fprintf(newFile, "%s\t", strng);
        }
 
	    for (int j = 0; j < newNumCols-1; j++) {
	      fscanf(output, "%s", strng); 
          if(feof(output))
            fatal("Output file is missing data");    
	      fprintf(newFile, "%s\t", strng);    

	      if (space) { // read in and write out second string
	        fscanf(output, "%s", strng);
          if(feof(output))
            fatal("Output file is missing data");     
	        fprintf(newFile, "%s\t", strng);   
	      }            
	   }
  
     fscanf(output, "%s", strng);
     if(feof(output))
       fatal("Output file is missing data");
	   fprintf(newFile,"%s\n", strng);
       
    }


      if(LOG_FILE)
	fprintf(logfile, "\n\n%d header rows, %d data rows,\n%d header columns, and %d data columns\nprinted to '%s'.\n",numheadrows, newNumRows, numheadcols, newNumCols, outputFileName);
      
      std::cout << "\n" << numheadrows << " header rows, " << newNumRows << " data rows," << std::endl;
      std::cout << numheadcols << " header columns, and " << newNumCols << " data columns" << std::endl;
      std::cout << "printed to '" << outputFileName << "'.\n" << std::endl;

      fclose(output);
      fclose(newFile);
      
      strcpy (inputFileName, "tempFile"); // make 'tempFile' the new input file
      //std::cout << "New input file: " << inputFileName << "\n\n";
      
      numRows = newNumRows;
      numCols = newNumCols;
  
  } // end of while (keepLooping)
  

  // remove tempfile
  char command [] = "rm tempFile";
  std::system(command);

  delete [] rowsMiss;
  delete [] colsMiss;
  delete [] threshRows;
  delete [] threshCols;



  t.stop("\nTimer stopped.");
  std::cout << t << " seconds.\n" << std::endl;

  double compTime;
  compTime = t.timeVal();

  if(LOG_FILE)
    fprintf(logfile, "\nTimer stopped.\n%f seconds.\n", compTime);

  fclose(logfile);

  return 0;
}





void checkConstants()
{
  // check Boolean values 

  if((LOG_FILE != 0) && (LOG_FILE != 1))
    fatal("LOG_FILE value in miss.h should be zero or one.");

  if((QUIET != 0) && (QUIET != 1))
    fatal("QUIET value in miss.h should be zero or one.");

  if((VERBOSE != 0) && (VERBOSE != 1))
    fatal("VERBOSE value in miss.h should be zero or one.");

  if((TOL > 0.001) || (TOL < -0.00000001))
    fatal("Invalid value for TOL in miss.h.");

  if(CUSTOM_MISSING == 1) {
    if(MISSING_SYMBOL == 63)
      fatal("Invalid definition for MISSING_SYMBOL, symbol already counted as missing.");
    if(MISSING_SYMBOL == 78)
      fatal("Invalid definition for MISSING_SYMBOL, symbol already counted as missing.");
    if(MISSING_SYMBOL == 88)
      fatal("Invalid definition for MISSING_SYMBOL, symbol already counted as missing.");
  }
}
