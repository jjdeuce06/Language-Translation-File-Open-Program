/***********HEADER*******************/
/*	GROUP 1:
		- Margo Bonal: bon8330@pennwest.edu
		- John Gerega: ger9822@pennwest.edu
		- Luke Ruffing: RUF96565@pennwest.edu
	COURSE: Language Translation - CMSC-4180-100
	PROGRAM NAME: Scanner 
*/
/***********END HEADER**************/

/*-----------File Overview: ------------------
- G1P2.c - cmd entrypoint to exectue program
- file_util.h - holds function declarations
- file_util.c - holds functions and processes
- scanner.c   - holds scanner logic
----------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"
#include "scanner.h"

// NOTE: run program using command prompt, i.e: G1P1.c file_util.c -o G1P1.exe

/*
 * main
 * - Entry point for the program.
 * - Determines which case applies based on argc (0, 1, or 2 filenames provided).
 * - Collects/validates filenames using helper functions from file_util.
 * - Opens required files, creates listing/temp files, copies contents, then closes/cleans up.
 */
int main(int argc, char *argv[]) {
	//initialize variables for scanner
	FILE_STATUS status;
	Token token;
	char buffer[128];// this needs to be token??
	
	//Print program header/banner
	header();

	status = handleArgs(argc, argv); //handle cmd input

	// If user chose to quit anywhere during filename handling, terminate program
	if (status == FILE_QUIT){ //decides to run program or not
		printf("Program Terminated\n");
		return 0;
	}	
	openFiles();
	start_up(inputFile, outputFile, listingFile);

	runScanner(token, inputFile, outputFile, listingFile, buffer);//build and run scanner
	
	wrapup();//clean up files
    
	return 0;
}
