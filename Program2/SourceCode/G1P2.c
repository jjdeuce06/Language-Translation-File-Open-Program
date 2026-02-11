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

// NOTE: run program using command prompt, i.e: G1P1.c file_util.c -o G1P1.exe

/*
 * main
 * - Entry point for the program.
 * - Determines which case applies based on argc (0, 1, or 2 filenames provided).
 * - Collects/validates filenames using helper functions from file_util.
 * - Opens required files, creates listing/temp files, copies contents, then closes/cleans up.
 */
int main(int argc, char *argv[]) {

	FILE_STATUS status;
	
	// // Print program header/banner
	 header();

	status = handleArgs(argc, argv);

	// If user chose to quit anywhere during filename handling, terminate program
	if (status == FILE_QUIT){ //decides to run program or not
		printf("Program Terminated\n");
		return 0;
	}
	
	build();


//where to call scanner?!?!
	// startup();
	// while(token != SCANEOF){
	// 	scanner();
	// 	//write to output
	// 	//write to listing file

	// }
	// wrapup();
    
	return 0;
}
// #include <stdio.h>
// #include "file_util.h"   // must contain Token enum + prototypes

// int main(void)
// {
//     FILE *in_file;
//     FILE *out_file = NULL;   // not used yet
//     FILE *list_file = NULL;  // not used yet

//     char buffer[128];
//     Token token;

//     /* open input file */
//     in_file = fopen("input.txt", "r");
//     if (in_file == NULL) {
//         printf("Could not open input file\n");
//         return 1;
//     }

//     /* first token */
//     token = scanner(buffer, in_file, out_file, list_file);

//     /* loop until EOF */
//     while (token != SCANEOF) {
//         printf("Token: %d   Lexeme: %s\n", token, buffer);
//         token = scanner(buffer, in_file, out_file, list_file);
//     }

//     /* print EOF token */
//     printf("Token: %d   Lexeme: %s\n", token, buffer);

//     fclose(in_file);
//     return 0;
// }
