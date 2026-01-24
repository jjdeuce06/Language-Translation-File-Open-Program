#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_util.h"
#define IN_EXTENSION ".in"
#define OUT_EXTENSION ".out"

void header(){
    printf("\n=====================================\n");
    printf("Language Translation File Open Program\n");
    printf("=====================================\n\n");
}

void outputMenu(){
    printf("\n------Pick Option-------\n");
    printf("--[O] Overwrite File  --\n");
    printf("--[N] New Output File --\n");
    printf("--[Q] Quit            --\n");
    printf("------------------------\n");
}

//-----------------CASES------------------------
void cmdArgs(int argc, char *argv[], FILE **input_file, FILE **output_file)
{
    if (argc == 1)
    {
        printf("CASE 1: \n");
        printf("Description:No command line parameters: just program name\n");
        noArgs(input_file, output_file);
    }
    else if (argc == 2)
    {
        printf("CASE 2: \n");
        printf("Description: One CMD argument: filename input only\n");
        oneArg(argv, output_file);
    }
    else if (argc == 3)
    {
        printf("CASE 3: \n");
        printf("Description: Two CMD arguments: filename input and filename output\n");
        twoArg(argv, input_file, output_file);
    }
}
//---------------------------------------------------
//----------------ARGUMENT HANDLING-------------------------
void noArgs(FILE **input_file, FILE **output_file)
{
    char str[100];
    printf("Enter your input file name: ");
    scanf("%s", str);
    handleInputExe(str, input_file, IN_EXTENSION);

    char str2[100];
    printf("Enter your output file name: ");
    scanf("%s", str2);
    handleOutputExe(str2, output_file, OUT_EXTENSION);

   // processFiles(); //logic to handle opend files
}


void oneArg(char *argv[], FILE **output_file){

    char str[100];
    printf("Enter your output file name: ");
    scanf("%s", str);
    handleOutputExe(str, output_file, OUT_EXTENSION);
    // processFiles(); //logic to handle opend files

}

void twoArg(char *argv[], FILE **input_file, FILE **output_file)
{
    char in[100];
    char out[100];

    // Copy command-line args into writable buffers
    strcpy(in, argv[1]);
    strcpy(out, argv[2]);

    // Handle input and output independently
    handleInputExe(in, input_file, IN_EXTENSION);
    handleOutputExe(out, output_file, OUT_EXTENSION);

    // processFiles(); //logic to handle opend files
}


//---------------------------------------------------

//-------------------HELPERS-------------------------
void handleInputExe(char* str, FILE **input_file, const char* exeType)
{
    char* extension_indicator = strchr(str, '.');

    if (str[0] == '\0')
        return;

    if (extension_indicator == NULL)
    {
        printf("is null, place extention...\n");
        strcat(str, exeType);
    }
    else
    {
        printf("Extention Detected\n");
    }

    *input_file = fopen(str, "r");

    if (*input_file == NULL)
    {
        printf("Error opening input file: %s (Does Not Exist)\n", str);
        repromptFile(str, input_file, exeType, 1);
    }
}
void handleOutputExe(char* str, FILE **output_file, const char* exeType)
{
    char* extension_indicator = strchr(str, '.');

    if (str[0] == '\0')
        return;

    if (extension_indicator == NULL)
    {
        printf("is null, place extention...\n");
        strcat(str, exeType);
        *output_file = fopen(str, "w");
    }
    else
    {
        printf("Extention Detected\n");
        outputChoice();
    }

    if (*output_file == NULL)
    {
        printf("Error opening output file: %s (Does Not Exist)\n", str);
        repromptFile(str, output_file, exeType, 2);
    }
}

void repromptFile(char* str, FILE **file_ptr, const char* exeType, int choice){
    while (*file_ptr == NULL)
    {
        printf("\n---Please re-enter a valid file name: ");
        scanf("%s", str);
        if (choice == 1)
            handleInputExe(str, file_ptr, exeType);
        else if (choice == 2){
            handleOutputExe(str, file_ptr, exeType);
            strcat(str, exeType);
            *file_ptr = fopen(str, "w");
        }
            
    }
}


void outputChoice(){
    outputMenu();
    char userChoice;
    scanf("%c", &userChoice);

    switch(userChoice){
        case 'O':
            printf("Overwriting file...\n");
            break;
        case 'N':
            printf("Creating new output file...\n");
            break;
        case 'Q':
            printf("Quitting program...\n");
            exit(0);
            break;
        default:
            printf("Invalid choice, please try again.\n");
            outputChoice();
            break;
    }


}