// Project 1 (cs5370).cpp : This file contains the 'main' function. Program execution begins and ends there.
// By : Dae-Gyue Han
// re-saved version

#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

//FUNCTION PROTOTYPES
void copyArray(int* arr1, int* arr2, int length);
void initializeCounter(int* counter, int length);
void incrementCounter(int counter[], int base, int length, int i);
void printCounter(int* counter, int length);

int main(int argc, char* argv[])
{

    //VARIABLE DECLARATIONS
    //string file_cases;
    //string file_controls;
    int num_genes;
    int num_cases;
    int num_controls;
    int num_patients;
    string file_name;
    string output_name;

    if (false)
    {
        file_name = "small_4genes_3ind.txt";
        num_genes = 4;
        num_cases = 2;
        num_controls = 1;
        output_name = "test.txt";
    }
    else if (false)
    {
        file_name = "small_4genes_8ind.txt";
        num_genes = 4;
        num_cases = 4;
        num_controls = 4;
        output_name = "test.txt";

    }
    else if (false)
    {
        file_name = "synth_1_10D_10N_20G.txt";
        num_genes = 20;
        num_cases = 10;
        num_controls = 10;
        output_name = "output_1_10D_10N_20G.txt";
    }
    else if (false)
    {
        file_name = "synth_2_10D_10N_20G.txt";
        num_genes = 20;
        num_cases = 10;
        num_controls = 10;
        output_name = "output_2_10D_10N_20G.txt";
    }
    else if (false)
    {
        file_name = "synth_3_10D_10N_20G.txt";
        num_genes = 20;
        num_cases = 10;
        num_controls = 10;
        output_name = "output_3_10D_10N_20G.txt";
    }
    else if (false)
    {
        file_name = "synth_4_10D_10N_20G.txt";
        num_genes = 20;
        num_cases = 10;
        num_controls = 10;
        output_name = "output_4_10D_10N_20G.txt";
    }
    else if (false)
    {
        file_name = "myers_0.3thresh_8239genes_175case_186control.txt";
        num_genes = 8239;
        num_cases = 175;
        num_controls = 186;
        output_name = "myers.txt";
    }
    else
    {
        file_name = argv[1];
        num_genes = atoi(argv[2]);
        num_cases = atoi(argv[3]);
        num_controls = atoi(argv[4]);
        output_name = argv[5];
    }

    string current_line;
    ifstream datafile;

    num_patients = num_cases + num_controls;

    //OPEN DATA FILE
    datafile.open(file_name.c_str()); //For some reason, the open method will only accept c-strings not strings.
    if (datafile.fail())
    {
        cout << "Data file not found! Terminating program . . ." << endl << endl;
        exit(1);
    }

    //SETTING UP 2D ARRAY FOR DATA
    int** data = new int* [num_genes];
    for (int i = 0; i < num_genes; i++)
    {
        data[i] = new int[num_patients];
    }

    //SETTING UP 1D ARRAYS FOR PATIENTS AND GENES
    string* patients = new string[num_patients];
    string* genes = new string[num_genes];

    //TEMPORARY STORAGE VARIABLE FOR STRINGS
    string s;

    for (int i = 0; i < num_genes + 1; i++)
    {

        getline(datafile, current_line);
        //cout << current_line << endl;
        istringstream iss(current_line);

        for (int j = 0; j < num_patients + 1; j++)
        {
            if (i == 0 && j == 0)
            {
                iss >> s;
            }
            else if (i == 0 && j > 0)
            {
                iss >> patients[j - 1];
            }
            else if (i != 0 && j == 0)
            {
                iss >> genes[i - 1];
            }
            else
            {
                iss >> s;
                data[i - 1][j - 1] = stoi(s);
            }
        }
    }

    //CLOSE DATA FILE
    datafile.close();

    //PRINT DATA (DIAGNOSTIC TOOL)
    if (false) //TOGGLE HERE
    {
        cout << endl << "Reading back data arrays . . ." << endl;

        cout << "gene" << "\t";
        for (int j = 0; j < num_patients; j++)
        {
            cout << patients[j] << "\t";
        }

        cout << endl;

        for (int i = 0; i < num_genes; i++)
        {
            cout << genes[i] << "\t";
            for (int j = 0; j < num_patients; j++)
            {
                cout << data[i][j] << "\t";
            }

            cout << endl;
        }

        cout << endl << "Patient array: " << endl;

        for (int i = 0; i < num_patients; i++)
        {
            cout << patients[i] << "\t";
        }

        cout << endl << endl << "Gene array: " << endl;

        for (int j = 0; j < num_genes; j++)
        {
            cout << genes[j] << "\t";
        }

        cout << endl;
    }

    //RELAXATION ARRAY
    double* relaxation_up = new double[num_genes];
    double* relaxation_down = new double[num_genes];

    int up_score;
    int down_score;

    for (int i = 0; i < num_genes; i++)
    {

        up_score = 0;
        down_score = 0;

        for (int j = 0; j < num_cases; j++) //alter the 'num_patients' to conform to case percentages later
        {
            if (data[i][j] == 1)
            {
                up_score++;
            }
            else if (data[i][j] == -1)
            {
                down_score++;
            }
        }

        relaxation_up[i] = (double)up_score / (double)num_cases; //modify later if needed
        relaxation_down[i] = (double)down_score / (double)num_cases; // modify later if needed
    }

    //PRINT RELAXATION ARRAYS (DIAGNOSTIC TOOL)
    if (false) //TOGGLE HERE
    {
        for (int i = 0; i < num_genes; i++)
        {
            cout << endl;
            cout << genes[i] << " up: " << relaxation_up[i] << endl;
            cout << genes[i] << " down: " << relaxation_down[i] << endl;
        }
        cout << endl;
    }

    //Starting up our base-3 counter to simulate our decision tree.
    int* pattern_counter = new int[num_genes];
    initializeCounter(pattern_counter, num_genes);

    //Creating an array to save the pattern of our best match.
    int* best_pattern = new int[num_genes];
    initializeCounter(best_pattern, num_genes);

    //VARIABLES FOR COMPARISON LOOPS
    int case_score = 0;
    int control_score = 0;
    double best = 0;
    double case_ratio = 0;
    double control_ratio = 0;
    double difference = 0;
    double best_case = 0;
    double best_control = 0;
    bool loop = true;
    bool pruning;

    cout << "Working . . ." << endl << endl;

    //COMPARISON LOOPS
    int last_digit_before = 0;
    int last_digit_after = 0;
    while (!(last_digit_before == 2 && last_digit_after == 0) && loop)
    {
        //Necessary for loop control
        last_digit_before = pattern_counter[num_genes - 1];

        //BRANCH AND BOUND CODE
        pruning = true;
        while (pruning)
        {
            pruning = false;
            for (int i = num_genes - 1; i >= 0; i--)
            {
                if (pattern_counter[num_genes - 1] == 2 && (best > relaxation_down[0]))
                {
                    //cout << "Triggering termination condition at: ";
                    //printCounter(pattern_counter, num_genes);

                    incrementCounter(pattern_counter, 3, num_genes, (num_genes - 1));
                    loop = false;
                    pruning = false;
                }
                else if (pattern_counter[i] == 1 && (best > relaxation_up[num_genes - 1 - i]))
                {
                    incrementCounter(pattern_counter, 3, num_genes, i);
                    pruning = true;
                }
                else if (pattern_counter[i] == 2 && (best > relaxation_down[num_genes - 1 - i]))
                {
                    incrementCounter(pattern_counter, 3, num_genes, i);
                    pruning = true;
                }
            }
        }

        //PRINT COUNTER (DIAGNOSTIC TOOL)
        //printCounter(pattern_counter, num_genes);

        case_score = 0;
        for (int j = 0; j < num_cases; j++) // j corresponds to disease cases
        {
            for (int i = num_genes - 1; i >= 0; i--) // i corresponds to rows (genes)
            {
                if (pattern_counter[i] == 1 && data[(num_genes - 1 - i)][j] != 1)
                {
                    case_score--;
                    break;
                }
                else if (pattern_counter[i] == 2 && data[(num_genes - 1 - i)][j] != -1)
                {
                    case_score--;
                    break;
                }
            } //end i loop
            case_score++;
        }

        control_score = 0;
        for (int k = (num_patients - num_cases); k < num_patients; k++) // k corresponds to control cases
        {
            for (int i = num_genes - 1; i >= 0; i--) // i corresponds to rows(genes)
            {
                //This line is tricky. This math is essentially forcing the counter to be read "backwards."
                if (pattern_counter[i] == 1 && data[(num_genes - 1 - i)][k] != 1)
                {
                    control_score--;
                    break;
                }
                else if (pattern_counter[i] == 2 && data[(num_genes - 1 - i)][k] != -1)
                {
                    control_score--;
                    break;
                }
            } //end i loop
            control_score++;
        }

        case_ratio = (double)case_score / (double)num_cases;
        control_ratio = (double)control_score / (double)num_controls;
        difference = case_ratio - control_ratio;

        if (difference > best)
        {
            best = difference;
            best_case = case_ratio;
            best_control = control_ratio;
            copyArray(pattern_counter, best_pattern, num_genes);
        }

        //ENUMERATION INCREMENTATION AND LOOP CONTROL
        incrementCounter(pattern_counter, 3, num_genes, 0);
        last_digit_after = pattern_counter[num_genes - 1];
    }

    //cout << "Optimal solution: " << best << endl;
    cout << "Base-3 pattern number: ";
    printCounter(best_pattern, num_genes);
    //cout << best_case * 100 << "% Cases, " << best_control * 100 << "% Controls" << endl << endl;

    //Creating an array of patients which match and are at-risk.
    int* risk = new int[num_patients];
    initializeCounter(risk, num_patients);
    risk[0] = 0;


    //POPULATING AT-RISK PATIENTS
    int risk_score = 0;
    for (int j = 0; j < num_patients; j++)
    {
        for (int i = num_genes - 1; i >= 0; i--) // i corresponds to rows (genes)
        {
            if (best_pattern[i] == 0)
            {
                risk_score++;
            }
            else if (best_pattern[i] == 1 && data[(num_genes - 1 - i)][j] == 1)
            {
                risk_score++;
            }
            else if (best_pattern[i] == 2 && data[(num_genes - 1 - i)][j] == -1)
            {
                risk_score++;
            }
        }

        if (risk_score == num_genes)
        {
            risk[j] = 1;
        }

        risk_score = 0;
    }



    ofstream outputfile;
    //OPEN OUTPUT FILE
    outputfile.open(output_name.c_str()); //For some reason, the open method will only accept c-strings not strings.
    if (outputfile.fail())
    {
        cout << "Output file could not be written! Terminating program . . ." << endl << endl;
        exit(1);
    }

    cout << "Expression Pattern:" << endl;
    outputfile << "Expression Pattern:" << endl;

    for (int i = num_genes - 1; i >= 0; i--)
    {
        if (best_pattern[i] == 1)
        {
            cout << genes[num_genes - 1 - i] << "\t";
            cout << "High" << endl;

            outputfile << genes[num_genes - 1 - i] << "\t";
            outputfile << "High" << endl;
        }
        else if (best_pattern[i] == 2)
        {
            cout << genes[num_genes - 1 - i] << "\t";
            cout << "Low" << endl;

            outputfile << genes[num_genes - 1 - i] << "\t";
            outputfile << "Low" << endl;
        }
    }

    cout << endl;
    outputfile << endl;

    cout << "Cases with Pattern:" << endl;
    outputfile << "Cases with Pattern:" << endl;

    for (int j = 0; j < num_cases; j++)
    {
        if (risk[j])
        {
            cout << patients[j] << "\t";
            outputfile << patients[j] << "\t";
        }
    }

    cout << endl << endl;
    outputfile << endl << endl;

    cout << "Controls with Pattern:" << endl;
    outputfile << "Controls with Pattern:" << endl;

    for (int j = num_cases; j < num_patients; j++)
    {
        if (risk[j])
        {
            cout << patients[j] << "\t";
            outputfile << patients[j] << "\t";
        }
    }

    cout << endl << endl;
    outputfile << endl << endl;

    cout << "J = " << best << endl;
    outputfile << "J = " << best << endl;

    //CLOSE OUTPUT FILE
    outputfile.close();

}//END OF MAIN PROGRAM

//FUNCTION DEFINITIONS
void copyArray(int* arr1, int* arr2, int length)
{
    for (int i = 0; i < length; i++)
    {
        arr2[i] = arr1[i];
    }
}

//This function sets the counter to a base-10 value of '1.'
void initializeCounter(int* counter, int length)
{
    counter[0] = 1;
    for (int i = 1; i < length; i++)
    {
        counter[i] = 0;
    }
}

//"counter*" is the counter array and "base" is the base digit of our counter
//For the purposes of this project we are using base-3.
//The argument 'i' tells the function to increment starting at the selected position.
//The 'i' argument is vital for branch-and-bound operations.
void incrementCounter(int* counter, int base, int length, int i)
{
    while (i < length && counter[i] == (base - 1))
    {
        counter[i] = 0;
        i = i + 1;
    }

    if (i < length)
    {
        counter[i] += 1;
    }
}

//The length corresponds to the number of digits stored in the array.
//Prints array in reverse order to be consistent with handwritten notation.
void printCounter(int* counter, int length)
{
    for (int i = 0; i < length; i++)
    {
        cout << counter[length - i - 1];
    }
    cout << endl;
}