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

	**NOTE**: What if file is .txt
-------End Instructions-------*/
#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

/*
 * main
 * - Entry point for the program.
 * - Determines which case applies based on argc (0, 1, or 2 filenames provided).
 * - Collects/validates filenames using helper functions from file_util.
 * - Opens required files, creates listing/temp files, copies contents, then closes/cleans up.
 */
int main(int argc, char *argv[]) {

	FILE_STATUS status = FILE_CONTINUE;
	
	// Print program header/banner
	header();

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
           
    

	// Copy contents from input into output/listing/temp files
	copyFileContents(); // copy file contents to other files

	// Close all open files
	files_close();      // Close files
   	
   	// Wrap up (temp deletion currently commented out in wrapup())
   	wrapup();          // Delete temp files module
    
	return 0;
}
