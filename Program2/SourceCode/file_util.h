#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#include <stdio.h>

#define MAX_FILENAME_LENGTH 260

	//error tracking varibles
	extern int lexicalErrorCount;
	extern int currentLine;

	//ENUMERATED COMPLETION CODEs
	typedef enum {
		FILE_CONTINUE = 1,
		FILE_QUIT     = 0
	} FILE_STATUS;

	typedef enum{
		FILE_DOES_NOT_EXIST,
		FILE_EXISTS
	} FILE_EXIST_STATUS;

	//GLOBAL FILE HANDLES
	extern FILE* inputFile;
	extern FILE* outputFile;
	extern FILE* listingFile;
	extern FILE* tempFile1;
	extern FILE* tempFile2;

	//GLOBAL FILE NAME VARIABLES
	extern char inputFileName[MAX_FILENAME_LENGTH];
	extern char outputFileName[MAX_FILENAME_LENGTH];
	extern char listingFileName[MAX_FILENAME_LENGTH];
	extern char tempFileName1[MAX_FILENAME_LENGTH];
	extern char tempFileName2[MAX_FILENAME_LENGTH];

    void header(); // prints header
    void outputMenu();// prints a menu used in output choice
    
    int outputChoice();// output file menu
	void openFiles(void); //open all files
	void build(void);

    FILE_STATUS handleArgs(int argc, char *argv[]);
    FILE_STATUS noArgs();// handles no arguments
    FILE_STATUS oneArg(const char* inputArg);// handles one arguments
    FILE_STATUS twoArg(const char* inputArg, const char* outputArg);// handles two arguments
    
    FILE_STATUS handleInputExe(char* str, const char* exeType);// handles in extension
    FILE_STATUS handleOutputExe(char* str, const char* exeType);//handles out extenstion
    
    FILE_STATUS repromptFile(char* str, const char* exeType, int choice);// reprompts for file name
	
	
	void copyFileContents();//copies file contents

	void backupOutputFile(const char *outputfilename);//backup old outputfile

    void createListingFileName();//creates listing file name
	void createTempFileNames();// creates temp file names

	//File Openers
	FILE* openInputFile();
	FILE* openOutputFile();
	FILE* openListingFile();
	FILE* openTempFile1();
	FILE* openTempFile2();
	
	FILE_EXIST_STATUS file_exists(const char* filename);//checks files existance
	
	void files_close();// closes files
	void wrapup();// removes temp files

	//fix returns
	typedef enum {
		OUTPUT_OVERWRITE,
		OUTPUT_NEW,
		OUTPUT_QUIT,
		OUTPUT_INVALID
	} OUTPUT_CHOICE;

#endif