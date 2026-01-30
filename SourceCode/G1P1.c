/***********HEADER*******************/
/*	GROUP 1:
		- Margo Bonal: bon8330@pennwest.edu
		- John Gerega: ger9822@pennwest.edu
		- Luke Ruffing: RUF96565@pennwest.edu
	COURSE: Language Translation - CMSC-4180-100
	PROGRAM NAME: FILE OPEN 
*/
/***********END HEADER**************/

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"

/* run this program using the command prompt */

/*
 * main
 * - Entry point for the program.
 * - Determines which case applies based on argc (0, 1, or 2 filenames provided).
 * - Collects/validates filenames using helper functions from file_util.
 * - Opens required files, creates listing/temp files, copies contents, then closes/cleans up.
 */
int main(int argc, char *argv[]) {

	FILE_STATUS status;
	
	// Print program header/banner
	header();

	status = handleArgs(argc, argv);


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
