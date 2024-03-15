/*   
 *
 *   extractCols.cpp 
 *
 *   Program to extract data columns that have the first row match strings on a list
 *
 *   Written by Sharlee Climer
 *   December 2011
 *
*/

#include"extractCols.h"

using namespace std;

int main(int argc, char ** argv)
{
  if (argc != 7)
    fatal("Usage:\n  extractCols list.txt numInList numDataRows numDataCols original.txt output.txt"); 

  timer t;
  t.start("Timer started");  


  FILE *inList; // contains the list of columns to output data for
  FILE *input; // contains all genotypes
  FILE *output; // will hold data for list of columns
  FILE *remainder; // will hold remaining data if WRITE_REMAINDER = 1

  cout << "Command line arguments: \n\t";
  for (int i = 0; i < argc; i++)
	cout << argv[i] << " ";
  cout << "\n" << endl;

  // check to be sure all files are available
  if ((inList = fopen(argv[1], "r")) == NULL)
    fatal("List of columns could not be opened");
  if (((input = fopen(argv[5], "r")) == NULL) || ((output = fopen(argv[6], "w")) == NULL))
    fatal("File could not be opened.\n");
 
  int numInList = atoi(argv[2]); // number of columns in list
  int numRows = atoi(argv[3]); // number of data rows (not including headers)
  int numCols = atoi(argv[4]); // number of data columns (not including headers)
  char strng[100]; // temporary storage of strings

  cout << "Assuming " << NUMHEADROWS << " header rows and " << NUMHEADCOLS << " header columns in input file." << endl;
  cout << "Assuming " << numRows << " rows and " << numCols << " columns of data in file (not including headers)." << endl;

  // check WRITE_REMAINDER is legit
  if(WRITE_REMAINDER == 1)
    fatal("WRITE_REMAINDER needs maintenance");

  if(WRITE_REMAINDER) {
    cout << "Remaining columns will be written to 'remainder.txt'.\n" << endl;
    if ((remainder = fopen("remainder.txt", "w")) == NULL)
      fatal("File for remainder columns could not be opened");
  }
  
  if (NUMHEADROWS != 1)
    fatal("Maintenance needed for more than one header row.");

  cout << "Extracting data for " << numInList << " columns listed in '" << argv[1] << "' from '" << argv[5] << "'...\n" << endl;

  // allocate memory for list of columns
  char **ind;
  if((ind = new char*[numInList]) == NULL)
    fatal("memory not allocated");
  for (int i = 0; i < numInList; i++) {
    if ((ind[i] = new char[100]) == NULL)
      fatal("memory not allocated");
  }

  // allocate memory for lengths of each columns label
  int *length;
  if((length = new int[numInList]) == NULL)
    fatal("memory not allocated");

  // read in and store list of columns
  for (int i = 0; i < numInList; i++) 
    fscanf(inList,"%s",ind[i]);

  fclose(inList);

  if (VERBOSE) {
    cout << "Column IDs to extract: " << endl;
    for (int i = 0; i < numInList; i++)
      cout << ind[i] << endl;
  }
  
  // record length of each string

  int num; // count number of characters
  for (int i = 0; i < numInList; i++) {
    num = 0;
    while (ind[i][num] != '\0')
      num++;
    if (num >= 100)
      fatal("String lengths too long.");
    length[i] = num; // number of chars in string
  }

  // read in header columns of first row and disregard until later
  for (int i = 0; i < NUMHEADCOLS; i++)
    fscanf(input, "%s", strng);

  // read in first row and determine column numbers to retain
  int *keepCol; // boolean value for keeping each data column
  if((keepCol = new int[numCols]) == NULL)
    fatal("memory not allocated");

  // intialize to zero
  for (int i = 0; i < numCols; i++)
    keepCol[i] = 0;

  // read in and match columns to keep
  int check = 0; // tally to see if number found equals number in list
  
  if (VERBOSE)
    cout << "\nIDs in data file:\n";

  for (int j = 0; j < numCols; j++) {
    fscanf(input, "%s", strng);
    
    if (VERBOSE) 
      cout << strng << " ";
    
    // see if this column is on the list
    for (int i = 0; i < numInList; i++) {
      int num = 0; // initialize number of chars
      while (strng[num] != '\0')
	num++;
      if (num >= 100)
	fatal("String length too long.");
      
      if ((num == length[i]) && (strncmp(strng, ind[i], length[i]) == 0)) {
	keepCol[j] = 1; // set flag as this column is in the list
	check++; // found another that is in list

	if(!QUIET)
	  cout << strng << endl;
	break;
      }
    }
  }

  if (!QUIET){
    cout << "\nData column numbers to be kept:" << endl;
    for (int i = 0; i < numCols; i++)
      if (keepCol[i] == 1)
	cout << i+1 << endl;
  }

  // check numbers are the same
  if(check != numInList)
    cout << "**Warning: " << numInList << " in list, but only " << check << " found in input file.**\n" << endl;

  fclose(input); // will reopen

  // reopen file and read in and write out selected data
  if ((input = fopen(argv[5], "r")) == NULL)
    fatal("Input file could not be opened.\n");

  // read in and write out header rows (not including ID row)
  if(NUMHEADROWS > 1) 
    for (int i = 0; i < NUMHEADROWS-1; i++) {
      for (int j = 0; j < NUMHEADCOLS; j++) {
	      fscanf(input, "%s", strng);
        if ((keepCol[j]) &&(WRITEHEADCOLS)) // only print out headers for columns that are kept
	        fprintf(output, "%s ", strng);

	if(WRITE_REMAINDER)
	  fprintf(remainder, "%s ", strng);
      }
  // read in and write out rest of header rows
  for (int j = 0; j < numCols; j++) {
	  fscanf(input, "%s", strng);

	  if (keepCol[j]) // only print out headers for columns that are kept
	    fprintf(output, "%s ", strng);

	  else if(WRITE_REMAINDER)
	    fprintf(remainder, "%s ", strng);
      }

      fprintf(output, "\n");

      if(WRITE_REMAINDER)
	fprintf(remainder, "\n");
    }

  // read in and write out ID row followed by data rows
  for (int i = 0; i < numRows+1; i++) {
    // check for premature end of file
    if(feof(input))
      fatal("Expected more data in input file.");

    for (int j = 0; j < NUMHEADCOLS; j++) { // write out header columns
      fscanf(input, "%s", strng);
      if (WRITEHEADCOLS)
        fprintf(output, "%s ", strng);

      if(WRITE_REMAINDER)
	fprintf(remainder, "%s ", strng);
    }

    for (int j = 0; j < numCols; j++) {
      if(feof(input))
	fatal("Expected more data in input file.");

      fscanf(input, "%s", strng);
      
      if (keepCol[j]) // only print out columns that are kept
	fprintf(output, "%s ", strng);

      else if(WRITE_REMAINDER)
	fprintf(remainder, "%s ", strng);
    }
    
    fprintf(output, "\n");

    if(WRITE_REMAINDER)
      fprintf(remainder, "\n");
  }

  // check for end of file
  if(!feof(input)) {
    fscanf(input, "%s", strng); // may have white space at end of file
    if(!feof(input))
      fatal("Additional unread data in input file.");
  }



  cout << "Data for " << check << " selected columns have been written to '" << argv[6] << "'.\n" << endl;

  fclose(input);
  fclose(output);

  if(WRITE_REMAINDER)
    fclose(remainder);

  for (int i = 0; i < numInList; i++)
    delete [] ind[i];
  delete [] ind;
  delete [] length;


  t.stop("\nTimer stopped");
  cout << t << " seconds\n" << endl;

  return 1;
}


