#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // for temp file naming
#include "file_util.h"
#define IN_EXTENSION ".in"
#define OUT_EXTENSION ".out"

//error tracking varibles
int lexicalErrorCount = 0;
int currentLine = 1;

//File declarations
FILE* inputFile = NULL;
FILE* outputFile = NULL;
FILE* listingFile = NULL;
FILE* tempFile1 = NULL;
FILE* tempFile2 = NULL;

//File name declarations to extract from the user
char inputFileName[MAX_FILENAME_LENGTH] = "";
char outputFileName[MAX_FILENAME_LENGTH] = "";
char listingFileName[MAX_FILENAME_LENGTH] = "";
char tempFileName1[MAX_FILENAME_LENGTH] = "";
char tempFileName2[MAX_FILENAME_LENGTH] = "";


//-------------------INTRO / UI HELPERS-------------------------
//Header function, produces an introduction to the program
//Purpose: Prints a formatted header banner that identifies what the program is.
void header(){
    printf("\n=====================================\n");
    printf("Language Translation Scanner Program\n");
    printf("=====================================\n\n");
}

//Provides the user an output menu of options if the output file they provide already exists
//Purpose: Displays the menu of choices when an output file name already exists on disk.
void outputMenu(){
    printf("\n------Pick Option-------\n");
    printf("--[O] Overwrite File  --\n");
    printf("--[N] New Output File --\n");
    printf("--[Q] Quit            --\n");
    printf("------------------------\n");
}

FILE_STATUS handleArgs(int argc, char *argv[]){

	FILE_STATUS status = FILE_CONTINUE;
	// CASE 1: No command line args besides program name -> prompt for input + output
	if (argc == 1)
    {
        printf("CASE 1: \n");
        printf("Description:No command line parameters: just program name\n");
        status = noArgs();
    }
    // CASE 2: One argument -> treat as input filename, prompt for output
    else if (argc == 2)
    {
        printf("CASE 2: \n");
        printf("Description: One CMD argument: filename input only\n");
        status = oneArg(argv[1]);
    }
    // CASE 3: Two arguments -> input + output provided
    else if (argc == 3)
    {
        printf("CASE 3: \n");
        printf("Description: Two CMD arguments: filename input and filename output\n");
        status = twoArg(argv[1], argv[2]);
	}
	
	// If user chose to quit anywhere during filename handling, terminate program
	if (status == FILE_QUIT){ //decides to run program or not
		printf("Program Terminated\n");
		return 0;
	}
	return status;
}

void openFiles(void){
	// OPEN FILES (input/output always required)
	inputFile = openInputFile();
	outputFile = openOutputFile();
	 
	// Report input open status
	if (inputFile == NULL) {
    	printf("Failed to open input file.%s\n", inputFileName);
   		
	}else{
    	printf("Input file opened successfully: %s\n", inputFileName);
	}
	
	// Report output open status
	if (outputFile == NULL){
    	printf("Failed to open output file.\n");
	}else{
    	printf("Output file opened: %s\n", outputFileName);
	}
	
	// If input/output opened, create/open listing + temp files
    if(inputFile && outputFile){
    	
    	createListingFileName();
    	createTempFileNames();
    	listingFile = openListingFile();
    	tempFile1 = openTempFile1();
    	tempFile2 = openTempFile2();
	}
}


//-------------------COMMAND LINE HANDLING-------------------------
//Function for when the user gives no files in the command prompts
//Purpose: Handles the case where the program is run with no command-line arguments.
FILE_STATUS noArgs() // 
{
	//character declarations to hold user input
    char str[100];
    char str2[100];
    	 
	//Prompt for the file name       
    printf("Enter your input file name: ");
    fgets(str, sizeof(str), stdin);    //reads input from the user and places the string into str
    str[strcspn(str, "\n")] = '\0';	   //removes trailing newline from str

	//check if the user simply hit the enter key
    if (str[0] == '\0')
	{
        return FILE_QUIT;  // if no input file entered when prompted terminate program
    }    

    
	//run str through handleInputExe and see if the function returns a FILE_QUIT
    if(handleInputExe(str, IN_EXTENSION) == FILE_QUIT)
	{
     //if it does, return status FILE_QUIT
     return FILE_QUIT;   	
	}

	//otherwise, prompt for the output file
    printf("Enter your output file name: ");
    fgets(str2, sizeof(str2), stdin);    //reads input from the user and places the string into str2
    str2[strcspn(str2, "\n")] = '\0';    //removes trailing newline from str

	//run str2 through handleOutputeExe and see if the function returns a FILE_QUIT
	if(handleOutputExe(str2, OUT_EXTENSION) == FILE_QUIT)
	{
		//if it does return status FILE_QUIT
    	return FILE_QUIT;
	}
	//if all the files return a good status, return FILE_CONTINUE
	return FILE_CONTINUE;
}

//Function for user providing one command line parameter
FILE_STATUS oneArg(const char* inputArg){

	//initialize one string str, and an inputName with a length of the predetermined maximum
	char str[100];
	char inputName[MAX_FILENAME_LENGTH];
	
	//place the inputArg as the value of inputName
    strcpy(inputName, inputArg);
	
	//run the input file name through the handleInputExe function
	if(handleInputExe(inputName, IN_EXTENSION) == FILE_QUIT)
	{
	 //return FILE_QUIT if the function returns a status of File_Quit
     return FILE_QUIT;   	
	}

	//prompt the user for an ouput file name
    printf("Enter your output file name: ");
    fgets(str, sizeof(str), stdin);    //reads input from the user and places the string into str
    str[strcspn(str, "\n")] = '\0';    //removes trailing newline from str

	//run the output file name through handleOutputExe
	if(handleOutputExe(str, OUT_EXTENSION) == FILE_QUIT)
	{
		//if the function returns FILE_QUIT, return FILE_QUIT from this function
    	return FILE_QUIT;
	}
	
	//if everything checks out, retur FILE_CONTINUE
	return FILE_CONTINUE;
}

//function for two command parameters provided by the user
//Purpose: Handles the case where the program is run with two command-line arguments.
FILE_STATUS twoArg(const char* inputArg, const char* outputArg)
{
	//initialize an input name character array, and place inputArg into it
    char inputName[MAX_FILENAME_LENGTH];
    strcpy(inputName, inputArg);
    
    //initialize an output name character array, and place outputArg into it
	char outputName[MAX_FILENAME_LENGTH];
    strcpy(outputName, outputArg);
    
    //run the inputName through handleInputExe
	if(handleInputExe(inputName, IN_EXTENSION) == FILE_QUIT)
	{
	 //if the function returns FILE_QUIT, return FILE_QUIT from this function
     return FILE_QUIT;   	
	}
    
    //run the outputName through handleOutputExe
    if(handleOutputExe(outputName, OUT_EXTENSION) == FILE_QUIT)
	{
		//if the function returns FILE_QUIT, return FILE_QUIT from this function
    	return FILE_QUIT;
	}
	
	//if everything checks out, return FILE_CONTINUE
	return FILE_CONTINUE;
}


//-------------------HELPERS-------------------------
//Function for handling input file names from the user
//Purpose: Validates and normalizes an input filename provided by the user (or command line).
FILE_STATUS handleInputExe(char* str, const char* exeType)
{
	//create a character named extension indicator, with the value being a pointer to the first occurrence of the . character
    char* extension_indicator = strchr(str, '.');

	//check if the string is empty
    if (str[0] == '\0')
	{
		//if it is, return FILE_QUIT
    	return FILE_QUIT;
	}

	//check if the extension indicator is null
    if (extension_indicator == NULL)
	{
		//if it is, print a message, and place the .in extension on the end and look for the file
        printf("No extension found, placing default extention...\n");
        strcat(str, exeType);
    }
    else
	{
		//otherwise, print that the extension was found
        printf("Extension Detected\n");
    }
	
	//while the file the user entered does not exist
	while(file_exists(str) == FILE_DOES_NOT_EXIST){
		
		//print error message and message to reprompt
		printf("Input file does not exist\n ");
		printf("---Please re-enter a valid file name: ");
		
		//get the new input the user entered and remove the trailing newline
		fgets(str, MAX_FILENAME_LENGTH, stdin);
		str[strcspn(str, "\n")] = '\0';
		
		//if the user enters nothing, return FILE_QUIT
		if(str[0] == '\0')
		{
			return FILE_QUIT;
		}
		
		//get the extension indicator in the file name
		extension_indicator = strchr(str, '.');
		//if there is no extension
        if (extension_indicator == NULL)
		{
        	//print a message and place the default extension
            printf("No extension found, placing default extention...\n");
            strcat(str, exeType);
        }
		else
		{
			//indicate an extension was found
            printf("Extention Detected\n");
        }
	}
	
	//once the user enters a valid name, print a message and copy the str into inputFileName, then return FILE_CONTINUE
	printf("INPUT FILE EXISTS\n");
	strcpy(inputFileName, str);
	return FILE_CONTINUE;
}

FILE_STATUS handleOutputExe(char* str, const char* exeType) {
    while (1) {
        // Remove newline
        str[strcspn(str, "\n")] = '\0';

        // If empty, default to input file name
        if (str[0] == '\0') {
            printf("NO OUTPUT FILE GIVEN, using input file name as base\n");
            strcpy(str, inputFileName);
        }

        // Check for extension
        char* dot = strrchr(str, '.');
        if (dot == NULL) {
            // No extension,  default
            strcat(str, exeType);
            printf("No extension found, adding default: %s\n", exeType);
        } else {
            printf("Extension detected\n");
        }

        // Prevent output matching input
        if (strcmp(str, inputFileName) == 0) {
            printf("Error: Output file cannot be the same as input file!\n");
            printf("Enter a different output file name: ");
            fgets(str, MAX_FILENAME_LENGTH, stdin);
            continue; //loop
        }

        // Check if file exists
        if (file_exists(str) == FILE_EXISTS) {
            printf("OUTPUT FILE EXISTS\n");
            int choice = outputChoice();

            if (choice == OUTPUT_OVERWRITE) {
                backupOutputFile(str); // move old output to .BAK (safe)
                break; // now safe to use
            } else if (choice == OUTPUT_NEW) {
                printf("Enter a new output file name: ");
                fgets(str, MAX_FILENAME_LENGTH, stdin);
                continue; // loop again
            } else if (choice == OUTPUT_QUIT) {
                return FILE_QUIT;
            }
        } else {
            // File does not exist, safe to use
            break;
        }
    }

    // Copy final safe name to global
    strcpy(outputFileName, str);
    return FILE_CONTINUE;
}

//Purpose: Checks whether a file exists by attempting to open it for reading.
FILE_EXIST_STATUS file_exists(const char* filename){
	FILE* file = fopen(filename, "r");  //  try opening it for reading
    if (file) {
        fclose(file);  //file exists and close it
        return FILE_EXISTS;      //true
    }
    return FILE_DOES_NOT_EXIST;          //false
}

//Purpose: Presents the overwrite/new/quit menu and returns a normalized numeric choice.
int outputChoice(){
   	outputMenu();
    char input[10];
    char userChoice;
    
    fgets(input, sizeof(input), stdin);  // uses fgets instead of scanf
    userChoice = input[0];  //gets first character

    switch(userChoice) {
        case 'O':
        case 'o':  //accept the owercases too
            printf("Overwriting file...\n");
            return OUTPUT_OVERWRITE;
        case 'N':
        case 'n':
            printf("Creating new output file...\n");
            return OUTPUT_NEW;
        case 'Q':
        case 'q':
            printf("Quitting program...\n");
            return OUTPUT_QUIT;
        default:
            printf("Invalid choice, please try again.\n");
            return outputChoice();
    }
}

//Purpose: Opens the validated input file (global inputFileName) for reading.
FILE* openInputFile(){
	
	FILE* file = fopen(inputFileName, "r"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open input file: %s\n", inputFileName);
        return NULL;  
    }
    return file;
}
	
//Purpose: Opens the validated output file (global outputFileName) for writing.
FILE* openOutputFile(){

	FILE* file = fopen(outputFileName, "w"); // open the file for reading
	if (file == NULL) {
		printf("Error: Could not open output file: %s\n", outputFileName);
		return NULL;  
	}
	return file;
}

//Purpose: Opens the listing file (global listingFileName) for writing.
FILE* openListingFile(){
	
	FILE* file = fopen(listingFileName, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open listing file: %s\n", listingFileName);
        return NULL;  
    }
    return file;
}

//Purpose: Opens temp file #1 (global tempFileName1) for writing.
FILE* openTempFile1(){
	
	FILE* file = fopen(tempFileName1, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 1: %s\n", tempFileName1);
        return NULL;  
    }
    return file;
}

//Purpose: Opens temp file #2 (global tempFileName2) for writing.
FILE* openTempFile2(){
	
	FILE* file = fopen(tempFileName2, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 2: %s\n", tempFileName2);
        return NULL;  
    }
    return file;
}

//Purpose: Copies the entire contents of the inputFile stream into output/listing/temp streams.
void copyFileContents(){ // copies the conents of the input to all the other files
	
	int input;
	 
    while ((input = fgetc(inputFile)) != EOF) //read until EOF
    {
    	printf("READ: %c\n", input); 
    	fputc(input, outputFile);
       	fputc(input, listingFile);
       	fputc(input, tempFile1);
       	fputc(input, tempFile2);
    }
	
}

//Purpose: Closes any currently open files and resets global file pointers to NULL.
void files_close() { // closes all the files
    if (inputFile) {
        fclose(inputFile);
        inputFile = NULL;
    }
    if (outputFile) {
        fclose(outputFile);
        outputFile = NULL;
    }
    if (listingFile) {
        fclose(listingFile);
        listingFile = NULL;
    }
    if (tempFile1) {
        fclose(tempFile1);
        tempFile1 = NULL;
    }
    if (tempFile2) {
        fclose(tempFile2);
        tempFile2 = NULL;
    }
    
    printf("All Files Closed.\n");
}

//Purpose: Creates a backup of an existing output file by renaming it to a .BAK filename.
void backupOutputFile(const char *outputfilename){
	
	char backupName[MAX_FILENAME_LENGTH];

    strcpy(backupName, outputfilename);

    char *dot = strrchr(backupName, '.');
    
    if (dot != NULL){
		strcpy(dot, ".BAK");
	}  
    else{
    	strcat(backupName, ".BAK");
	}
        
    remove(backupName);              // remove old file backup if it exists
    rename(outputfilename, backupName);  // replaces the file name with the same name but as a .BAK
    
}

//Purpose: Derives the listing file name from the chosen output file name.
void createListingFileName(void)
{
    strcpy(listingFileName, outputFileName);

    char *dot = strrchr(listingFileName, '.');
    if (dot != NULL){
    	strcpy(dot, ".LIS");
	}   
    else{
    	strcat(listingFileName, ".LIS");
	}
        
}

//Purpose: Generates unique temporary file names for tempFileName1 and tempFileName2.
void createTempFileNames(void)
{
	srand(time(NULL));
	int random_num = (rand() % 10000) + 1;
	
	sprintf(tempFileName1, "tempfile1_%d", random_num);
	sprintf(tempFileName2, "tempfile2_%d", random_num);
	
    char *dot1 = strrchr(tempFileName1, '.');
    char *dot2 = strrchr(tempFileName2, '.');

    if (dot1 != NULL){
    	strcpy(dot1, ".TMP1");
	}   
    else{
    	strcat(tempFileName1, ".TMP1");
	}

    if (dot2 != NULL){
    	strcpy(dot2, ".TMP2");
	}   
    else{
    	 strcat(tempFileName2, ".TMP2");
	}
       
}

//Purpose: Performs end-of-program cleanup tasks (currently temporary file deletion is disabled).
void wrapup(){

	files_close();
	
	printf("Wrap up complete. (temporary file deletion commented out in wrap up module)\n");
}
