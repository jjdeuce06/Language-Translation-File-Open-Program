#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_util.h"
#define IN_EXTENSION ".in"
#define OUT_EXTENSION ".txt"


FILE* inputFile = NULL;
FILE* outputFile = NULL;
FILE* listingFile = NULL;
FILE* tempFile1 = NULL;
FILE* tempFile2 = NULL;

char inputFileName[MAX_FILENAME_LENGTH] = "";
char outputFileName[MAX_FILENAME_LENGTH] = "";
char listingFileName[MAX_FILENAME_LENGTH] = "";
char tempFileName1[MAX_FILENAME_LENGTH] = "";
char tempFileName2[MAX_FILENAME_LENGTH] = "";


void header(){
    printf("\n=====================================\n");
    printf("Language Translation File Open Program\n");
    printf("=====================================\n\n");
}


void intialization(int argc, char *argv[], FILE **input_file, FILE **output_file)
{
  
}

int noArgs()
{
    char str[100];
    char str2[100];
    	        
    printf("Enter your input file name: ");
    scanf("%s", str);
    if(handleInputExe(str, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}



    
    printf("Enter your output file name: ");
    scanf("%s", str2);
	if(handleOutputExe(str2, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
}

int oneArg(const char* inputArg){

	if(handleInputExe(inputArg, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}

    char str[100];
    printf("Enter your output file name: ");
    scanf("%s", str);
	if(handleOutputExe(str, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
}

int twoArg(const char* inputArg, const char* outputArg)
{
    if(handleInputExe(inputArg, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}

    if(handleOutputExe(outputArg, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
}

//-------------------HELPERS-------------------------
int handleInputExe(char* str, const char* exeType)
{
    char* extension_indicator = strchr(str, '.');

    if (str[0] == '\0')
        return FILE_QUIT;

    if (extension_indicator == NULL)
    {
        printf("is null, place extention...\n");
        strcat(str, exeType);
    }
    else
    {
        printf("Extention Detected\n");
    }

	
	if(file_exists(str)){
		printf("INPUT FILE EXISTS\n");
	}else{
		printf("\n---Please re-enter a valid file name: ");
		repromptFile(str, exeType, 1);
	}
	
	strcpy(inputFileName, str);
	
}

int handleOutputExe(char* str, const char* exeType)
{
   
    char* extension_indicator = strchr(str, '.'); // looks for a "." in the filename
    
    if (str[0] == '\0') // if string is empty return
    	return FILE_QUIT;
    
    if (extension_indicator == NULL) // no "." was found ie no extension found in file name
    {
        printf("is null, place extention...\n");
        strcat(str, exeType);
	}
	else //if extension_indicator is not null then there is some type of extension on filename
    {
        printf("Extention Detected\n"); //notice extension is found
    }
    
    
    
	if(file_exists(str)){
		printf("OUTPUT FILE EXISTS\n");
		int i;
		i = outputChoice();
		if(i == 1){
			printf("You chose overwrite\n");
		}else if(i == 2){
			printf("You chose new output file\n");
			printf("\n---Please enter a valid new output file name: ");
			repromptFile(str, exeType, 2);
		}else if(i ==3){
			printf("You chose exit\n");
			return FILE_QUIT;
		}

		}
	strcpy(outputFileName, str);
}

int file_exists(const char* filename){
	FILE* file = fopen(filename, "r");  //try opening for reading
    if (file) {
        fclose(file);  //file exists, close it
        return 1;      //true
    }
    return 0;          //false
}

void repromptFile(char* str, const char* exeType, int choice){
   
        
        scanf("%s", str);
        if (choice == 1){
        	handleInputExe(str, exeType);
        }else if (choice == 2){
        	handleOutputExe(str, exeType);
            //strcat(str, exeType);  //this just keeps throwing .out on the end however times reprompted
            //*file_ptr = fopen(str, "w");
        }
            
    
}

void outputMenu(){
    printf("\n------Pick Option-------\n");
    printf("--[O] Overwrite File  --\n");
    printf("--[N] New Output File --\n");
    printf("--[Q] Quit            --\n");
    printf("------------------------\n");
}

int outputChoice(){
    outputMenu();
    char userChoice;
    scanf("%c", &userChoice);

    switch(userChoice){
        case 'O':
            printf("Overwriting file...\n");
            return 1;
        case 'N':
            printf("Creating new output file...\n");
            return 2;
        case 'Q':
            printf("Quitting program...\n");
            //exit(0); //no exits allowed
            return 3;
        default:
            printf("Invalid choice, please try again.\n");
            outputChoice();
            break;
    }


}

FILE* openInputFile(){
	
	FILE* file = fopen(inputFileName, "r"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open input file: %s\n", inputFileName);
        return NULL;  
    }
    
    return file;
}
	
FILE* openOutputFile(){
	
	FILE* file = fopen(outputFileName, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open output file: %s\n", inputFileName);
        return NULL;  
    }
    
    return file;
}

void copyFileContents(){
	
	int input;
	
	 
    while ((input = fgetc(inputFile)) != EOF) //read until EOF
    {
    	printf("READ: %c\n", input); 
        fputc(input, outputFile);
       // fputc(input, listingFile);
       // fputc(input, tempFile1);
       // fputc(input, tempFile2);
       // printf("copying...\n");
    }
	
}

void files_close() {
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
}