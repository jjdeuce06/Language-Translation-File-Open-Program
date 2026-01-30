#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // for temp file naming
#include "file_util.h"
#define IN_EXTENSION ".in"
#define OUT_EXTENSION ".out"


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
/*---Instructions:---
	Three possible cases:
	1. No arguments only program name
	2. One argument - input file name
	3. Two arguments - input and output file names

	when extention not provided, default to .in for input and .out for output
	handle the cmd arguments

	Missing parameters:
	-prompt for missing 1 parameter
	-prompt for input name and output name
	-when 2 included no prompt

	Responses:
	-if extention included, use
	-if not not included, add default extentions
-------End Instructions-------*/

//Header function, produces an introduction to the program
//Purpose: Prints a formatted header banner that identifies what the program is.
//Behavior: Writes static text to stdout; does not read input or modify any global state.
//Parameters: None.
//Returns: None.
void header(){
    printf("\n=====================================\n");
    printf("Language Translation File Open Program\n");
    printf("=====================================\n\n");
}

//Provides the user an output menu of options if the output file they provide already exists
//Purpose: Displays the menu of choices when an output file name already exists on disk.
//Behavior: Writes menu text to stdout; the user's selection is read elsewhere (outputChoice()).
//Parameters: None.
//Returns: None.
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

















//-------------------COMMAND LINE HANDLING-------------------------
//Function for when the user gives no files in the command prompts
//Purpose: Handles the case where the program is run with no command-line arguments.
//Behavior:
//  - Prompts the user for an input file name, trims newline, and validates it via handleInputExe().
//  - Prompts the user for an output file name, trims newline, and validates/handles it via handleOutputExe().
//  - If the user presses Enter at the input prompt (empty string), the program quits.
//  - If either handler returns FILE_QUIT, this function returns FILE_QUIT.
//Side effects:
//  - On success, handleInputExe() copies the validated input name into global inputFileName.
//  - On success, handleOutputExe() copies the validated output name into global outputFileName.
//Parameters: None.
//Returns:
//  - FILE_QUIT if the user cancels or validation fails in a quitting way.
//  - FILE_CONTINUE if both input and output filenames are successfully processed.
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
//Purpose: Handles the case where the program is run with exactly one command-line argument.
//Behavior:
//  - Treats inputArg as the input file name and validates it via handleInputExe() (adds default extension if needed).
//  - Prompts the user for the output file name and validates/handles it via handleOutputExe().
//Side effects:
//  - On success, handleInputExe() copies the validated input name into global inputFileName.
//  - On success, handleOutputExe() copies the validated output name into global outputFileName.
//Parameters:
//  - inputArg: The input filename provided on the command line.
//Returns:
//  - FILE_QUIT if the user cancels or validation indicates quitting.
//  - FILE_CONTINUE if input and output filenames are successfully processed.
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
//Behavior:
//  - Treats inputArg as the input filename and validates it via handleInputExe().
//  - Treats outputArg as the output filename and validates/handles it via handleOutputExe().
//Side effects:
//  - On success, handleInputExe() copies the validated input name into global inputFileName.
//  - On success, handleOutputExe() copies the validated output name into global outputFileName.
//Parameters:
//  - inputArg: Input filename provided on the command line.
//  - outputArg: Output filename provided on the command line.
//Returns:
//  - FILE_QUIT if the user cancels or validation indicates quitting.
//  - FILE_CONTINUE if both filenames are successfully processed.
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
//Behavior:
//  - If the filename is empty, returns FILE_QUIT.
//  - Checks for an extension by searching for '.' using strchr().
//  - If no extension is found, appends the default extension (exeType) to the filename.
//  - Re-prompts the user in a loop until file_exists() returns true for the given filename.
//  - If the user enters an empty string during re-prompt, returns FILE_QUIT.
//Side effects:
//  - Mutates the passed-in string buffer (str) by appending exeType or replacing input contents.
//  - Copies the validated, existing filename into the global inputFileName.
//Parameters:
//  - str: Mutable string buffer containing the input filename; updated in-place.
//  - exeType: Default extension to append if none exists (e.g., ".in").
//Returns:
//  - FILE_QUIT if the user cancels/enters an empty string.
//  - FILE_CONTINUE if a valid existing file is selected and stored in inputFileName.
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
	while(!file_exists(str)){
		
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

//function to handle output files
//Purpose: Validates and normalizes an output filename provided by the user (or command line).
//Behavior:
//  - If the provided string is empty, derives an output filename from inputFileName by replacing/adding exeType.
//  - Ensures the filename has an extension (appends exeType if none exists).
//  - If the output file already exists, displays a menu and handles user choice:
//      * Overwrite: backs up existing output file via backupOutputFile().
//      * New: prompts for a new output filename and re-validates via recursion.
//      * Quit: returns FILE_QUIT.
//  - On success, copies the final output filename into the global outputFileName.
//Side effects:
//  - Mutates the passed-in string buffer (str) as it builds/changes the output filename.
//  - May rename existing output file to a .BAK via backupOutputFile().
//  - Sets global outputFileName when a final choice is made.
//Parameters:
//  - str: Mutable string buffer containing the output filename; updated in-place.
//  - exeType: Default extension to append if none exists (e.g., ".out").
//Returns:
//  - FILE_QUIT if the user chooses to quit from the menu.
//  - FILE_CONTINUE if a valid output filename is selected and stored in outputFileName.
FILE_STATUS handleOutputExe(char* str, const char* exeType)
{
	
   if (str[0] == '\0')
   {    // if string is empty give source file name with .out extension
    	printf("NO OUTPUT FILE GIVEN\n");
    	strcpy(str, inputFileName);

    	char *dot = strrchr(str, '.');
    	
    	if (dot != NULL){
			strcpy(dot, exeType);
		}else{
			strcat(str, exeType);		
		}    	
	}
	
	
    char* extension_indicator = strchr(str, '.'); // looks for a "." in the filename
    
    
    
    if (extension_indicator == NULL){ // no "." was found ie no extension found in file name
     printf("No extention found, placing default extention...\n");
        strcat(str, exeType);
        
	}else{ //if extension_indicator is not null then there is some type of extension on filename
        printf("Extention Detected\n"); //notice extension is found
    }
    
    
    
	if(file_exists(str)){
		printf("OUTPUT FILE EXISTS\n");
		int i = outputChoice();
		
		if(i == 1){
			//printf("You chose overwrite\n");
			backupOutputFile(str);
		}
		else if(i == 2){
			//printf("You chose new output file\n");
			printf("\n---Please enter a valid new output file name: ");
				
			fgets(str, MAX_FILENAME_LENGTH, stdin);
			str[strcspn(str, "\n")] = '\0';
		
			while(str[0] == '\0'){
				printf("New Output filename cannot be empty. Please enter a valid filename: ");
            	fgets(str, MAX_FILENAME_LENGTH, stdin);
            	str[strcspn(str, "\n")] = '\0';
			}
				
				return handleOutputExe(str, exeType); //use recurision to check name
		}else if(i ==3){
		
		//	printf("You chose exit\n");
			return FILE_QUIT;
		}

	}
	strcpy(outputFileName, str);
	return FILE_CONTINUE;
}

//Purpose: Checks whether a file exists by attempting to open it for reading.
//Behavior:
//  - Uses fopen(filename, "r") to try to open the file.
//  - If fopen succeeds, closes the file immediately and returns 1.
//  - If fopen fails, returns 0.
//Parameters:
//  - filename: Path/name of the file to test.
//Returns:
//  - 1 if the file can be opened for reading (exists and is accessible).
//  - 0 if it cannot be opened (does not exist or permission issue).
int file_exists(const char* filename){
	FILE* file = fopen(filename, "r");  //  try opening it for reading
    if (file) {
        fclose(file);  //file exists and close it
        return 1;      //true
    }
    return 0;          //false
}

//Purpose: Presents the overwrite/new/quit menu and returns a normalized numeric choice.
//Behavior:
//  - Calls outputMenu() to print the menu.
//  - Reads a line of input using fgets and takes the first character as the command.
//  - Accepts both uppercase and lowercase characters.
//  - For invalid input, prints an error and recursively calls itself until a valid choice is made.
//Parameters: None.
//Returns:
//  - 1 for overwrite (O/o)
//  - 2 for new output filename (N/n)
//  - 3 for quit (Q/q)
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
            return 1;
        case 'N':
        case 'n':
            printf("Creating new output file...\n");
            return 2;
        case 'Q':
        case 'q':
            printf("Quitting program...\n");
            return 3;
        default:
            printf("Invalid choice, please try again.\n");
            return outputChoice();
    }
}

//Purpose: Opens the validated input file (global inputFileName) for reading.
//Behavior:
//  - Attempts fopen(inputFileName, "r").
//  - If it fails, prints an error message and returns NULL.
//Parameters: None (uses global inputFileName).
//Returns:
//  - A FILE* handle opened for reading on success.
//  - NULL on failure.
FILE* openInputFile(){
	
	FILE* file = fopen(inputFileName, "r"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open input file: %s\n", inputFileName);
        return NULL;  
    }
    
    return file;
}
	
//Purpose: Opens the validated output file (global outputFileName) for writing.
//Behavior:
//  - Attempts fopen(outputFileName, "w") which truncates/creates the file.
//  - If it fails, prints an error message and returns NULL.
//Parameters: None (uses global outputFileName).
//Returns:
//  - A FILE* handle opened for writing on success.
//  - NULL on failure.
FILE* openOutputFile(){
	
	FILE* file = fopen(outputFileName, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open output file: %s\n", outputFileName);
        return NULL;  
    }
    
    return file;
}

//Purpose: Opens the listing file (global listingFileName) for writing.
//Behavior:
//  - Attempts fopen(listingFileName, "w") which truncates/creates the file.
//  - If it fails, prints an error message and returns NULL.
//Parameters: None (uses global listingFileName).
//Returns:
//  - A FILE* handle opened for writing on success.
//  - NULL on failure.
FILE* openListingFile(){
	
	
	FILE* file = fopen(listingFileName, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open listing file: %s\n", listingFileName);
        return NULL;  
    }
    
    return file;
}

//Purpose: Opens temp file #1 (global tempFileName1) for writing.
//Behavior:
//  - Attempts fopen(tempFileName1, "w") which truncates/creates the file.
//  - If it fails, prints an error message and returns NULL.
//Parameters: None (uses global tempFileName1).
//Returns:
//  - A FILE* handle opened for writing on success.
//  - NULL on failure.
FILE* openTempFile1(){
	
	FILE* file = fopen(tempFileName1, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 1: %s\n", tempFileName1);
        return NULL;  
    }
    
    return file;
}

//Purpose: Opens temp file #2 (global tempFileName2) for writing.
//Behavior:
//  - Attempts fopen(tempFileName2, "w") which truncates/creates the file.
//  - If it fails, prints an error message and returns NULL.
//Parameters: None (uses global tempFileName2).
//Returns:
//  - A FILE* handle opened for writing on success.
//  - NULL on failure.
FILE* openTempFile2(){
	
	FILE* file = fopen(tempFileName2, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 2: %s\n", tempFileName2);
        return NULL;  
    }
    
    return file;
}

//Purpose: Copies the entire contents of the inputFile stream into output/listing/temp streams.
//Behavior:
//  - Reads characters one at a time from global inputFile using fgetc() until EOF.
//  - Writes each character to outputFile, listingFile, tempFile1, and tempFile2 using fputc().
//  - Prints each character read to stdout for debugging ("READ: %c").
//Assumptions:
//  - inputFile, outputFile, listingFile, tempFile1, and tempFile2 are all valid open FILE* handles.
//Parameters: None (uses global FILE* variables).
//Returns: None.
void copyFileContents(){ // copies the conents of the input to all the other files
	
	int input;
	
	 
    while ((input = fgetc(inputFile)) != EOF) //read until EOF
    {
    	printf("READ: %c\n", input); 
    	fputc(input, outputFile);
       	fputc(input, listingFile);
       	fputc(input, tempFile1);
       	fputc(input, tempFile2);
      	 //printf("copying...\n");
    }
	
}

//Purpose: Closes any currently open files and resets global file pointers to NULL.
//Behavior:
//  - For each global FILE* (inputFile/outputFile/listingFile/tempFile1/tempFile2):
//      * If non-NULL, closes it with fclose() and sets the pointer to NULL.
//  - Prints "All Files Closed." after completing closure attempts.
//Parameters: None (uses global FILE* variables).
//Returns: None.
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
//Behavior:
//  - Copies outputfilename into a local buffer backupName.
//  - Finds the last '.' and replaces the extension with ".BAK" (or appends ".BAK" if none).
//  - Removes any existing backup file with the same name using remove().
//  - Renames the original output file to the backup name using rename().
//Side effects:
//  - Deletes any existing backupName file.
//  - Renames (moves) the original outputfilename to backupName.
//Parameters:
//  - outputfilename: The path/name of the output file to back up.
//Returns: None.
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
//Behavior:
//  - Copies global outputFileName into global listingFileName.
//  - Replaces the last extension (if any) with ".LIS" or appends ".LIS" if no '.' found.
//Side effects:
//  - Updates global listingFileName.
//Parameters: None (uses global outputFileName and updates global listingFileName).
//Returns: None.
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
//Behavior:
//  - Seeds the random number generator with current time.
//  - Generates a random number in range [1, 10000].
//  - Builds base names "tempfile1_<rand>" and "tempfile2_<rand>" via sprintf().
//  - Attempts to replace an existing extension if present (unlikely here) or appends ".TMP1"/".TMP2".
//Side effects:
//  - Updates global tempFileName1 and tempFileName2.
//Parameters: None (updates global temp file name buffers).
//Returns: None.
void createTempFileNames(void)
{
	srand(time(NULL));
	int random_num = (rand() % 10000) + 1;
	
   // strcpy(tempFileName1, outputFileName);
   // strcpy(tempFileName2, outputFileName);

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
//Behavior:
//  - Contains commented-out remove() calls for deleting temp files.
//  - Prints a message indicating wrap-up is complete and that deletion is commented out.
//Parameters: None.
//Returns: None.
void wrapup(){

	//remove(tempFileName1);
	//remove(tempFileName2);
	
	printf("Wrap up complete. (temporary file deletion commented out in wrap up module)\n");
}
