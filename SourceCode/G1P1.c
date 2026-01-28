/***********HEADER*******************/
/*	GROUP 1:
		- Margo Bonal: bon8330@pennwest.edu
		- John Gerega: ger9822@pennwest.edu
		- Luke Ruffing: RUF96565@pennwest.edu
	COURSE: Language Translation - CMSC-4180-100
	PROGRAM NAME: FILE OPEN 
*/
/***********END HEADER**************/

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
#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"

/* run this program using the console*/

int main(int argc, char *argv[]) {

	FILE_STATUS status = FILE_CONTINUE;
	
	header();

	
	if (argc == 1)
    {
        printf("CASE 1: \n");
        printf("Description:No command line parameters: just program name\n");
        status = noArgs();
    }
    else if (argc == 2)
    {
        printf("CASE 2: \n");
        printf("Description: One CMD argument: filename input only\n");
        status = oneArg(argv[1]);
    }
    else if (argc == 3)
    {
        printf("CASE 3: \n");
        printf("Description: Two CMD arguments: filename input and filename output\n");
        status = twoArg(argv[1], argv[2]);
	}
	
	if (status == FILE_QUIT){ //decides to run program or not
		printf("Program Terminated\n");
		return 0;
	}
	
	
	//OPEN FILES
	inputFile = openInputFile();
	outputFile = openOutputFile();
	 
	if (inputFile == NULL) {
    	printf("Failed to open input file.%s\n", inputFileName);
   		
	}else{
    printf("Input file opened successfully: %s\n", inputFileName);
	}
	
	
	if (outputFile == NULL){
    	printf("Failed to open output file.\n");
	}else{
    	printf("Output file opened: %s\n", outputFileName);
	}
	
    if(inputFile && outputFile){
    	
    	createListingFileName();
    	createTempFileNames();
    	
    	listingFile = openListingFile();
    	tempFile1 = openTempFile1();
    	tempFile2 = openTempFile2();
    	
	}
           
	copyFileContents(); // copy file contents to other files
	files_close();      // Close files
   	
   	wrapup();          // Delete temp files module
    
	return 0;
}