#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#include <stdio.h>

#define MAX_FILENAME_LENGTH 260

	typedef enum {
		
    FILE_CONTINUE = 1,
    FILE_QUIT     = 0
    
	} FILE_STATUS;

	extern FILE* inputFile;
	extern FILE* outputFile;
	extern FILE* listingFile;
	extern FILE* tempFile1;
	extern FILE* tempFile2;

	extern char inputFileName[MAX_FILENAME_LENGTH];
	extern char outputFileName[MAX_FILENAME_LENGTH];
	extern char listingFileName[MAX_FILENAME_LENGTH];
	extern char tempFileName1[MAX_FILENAME_LENGTH];
	extern char tempFileName2[MAX_FILENAME_LENGTH];

    void header(); // prints header
    void outputMenu();
   // void intialization();//NEW handles the arguments
    //void cmdArgs(int argc, char *argv[], FILE **input_file, FILE **output_file); //reads how many arguments
    
    FILE_STATUS handleInputExe(char* str, const char* exeType);// handles in extension
    FILE_STATUS handleOutputExe(char* str, const char* exeType);//handles out extenstion
    
    void repromptFile(char* str, const char* exeType, int choice);// reprompts for file name
	
	
	void copyFileContents();

	void backupOutputFile(const char *outputfilename);

    FILE_STATUS noArgs();
    FILE_STATUS oneArg(const char* inputArg);
    FILE_STATUS twoArg(const char* inputArg, const char* outputArg);

    int outputChoice();// output file menu
	

	
	FILE* openInputFile();
	FILE* openOutputFile();
	FILE* openListingFile();
	FILE* openTempFile1();
	FILE* openTempFile2();
	
	void files_close();
	
	int file_exists(const char* filename);
	
	void createListingFileName();
	void createTempFileNames();
	
	
    
#endif