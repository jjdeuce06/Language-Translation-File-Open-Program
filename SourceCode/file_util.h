#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#include <stdio.h>

#define MAX_FILENAME_LENGTH 260
#define FILE_CONTINUE 0
#define FILE_QUIT 1

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

    void header(); // pritns header
    void outputMenu();
   // void intialization();//NEW handles the arguments
    //void cmdArgs(int argc, char *argv[], FILE **input_file, FILE **output_file); //reads how many arguments
    int handleInputExe(char* str, const char* exeType);// handles in extension
    int handleOutputExe(char* str, const char* exeType);//handles out extenstion
    void repromptFile(char* str, const char* exeType, int choice);// reprompts for file name
	
	
	void copyFileContents();


    int noArgs();
    int oneArg(const char* inputArg);
    int twoArg(const char* inputArg, const char* outputArg);

    int outputChoice();// output file menu
	
	//void get_file_name();
	
	FILE* openInputFile();
	FILE* openOutputFile();
	//void file_open();
	void files_close();
	
	int file_exists(const char* filename);
    
#endif