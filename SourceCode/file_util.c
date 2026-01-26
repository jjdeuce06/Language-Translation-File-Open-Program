#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_util.h"
#define IN_EXTENSION ".in"
#define OUT_EXTENSION ".out"


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

FILE_STATUS noArgs() // 
{
    char str[100];
    char str2[100];
    	        
    printf("Enter your input file name: ");
    fgets(str, sizeof(str), stdin);    //reads input and doesnt skip whitespace
    str[strcspn(str, "\n")] = '\0';

    if (str[0] == '\0'){
        return FILE_QUIT;  // if no input file entered when prompted terminate program
    }    
        
    if(handleInputExe(str, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}



    
    printf("Enter your output file name: ");
    fgets(str2, sizeof(str2), stdin);    //reads input and doesnt skip whitespace
    str2[strcspn(str2, "\n")] = '\0';

	if(handleOutputExe(str2, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
	
	return FILE_CONTINUE;
}

FILE_STATUS oneArg(const char* inputArg){

	char inputName[MAX_FILENAME_LENGTH];
    strcpy(inputName, inputArg);
	
	if(handleInputExe(inputName, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}

    char str[100];
    printf("Enter your output file name: ");
    scanf("%s", str);
	if(handleOutputExe(str, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
	
	return FILE_CONTINUE;
}

FILE_STATUS twoArg(const char* inputArg, const char* outputArg)
{
    char inputName[MAX_FILENAME_LENGTH];
    strcpy(inputName, inputArg);
	
	if(handleInputExe(inputName, IN_EXTENSION) == FILE_QUIT){
     return FILE_QUIT;   	
	}

	char outputName[MAX_FILENAME_LENGTH];
    strcpy(outputName, outputArg);
    
    if(handleOutputExe(outputName, OUT_EXTENSION) == FILE_QUIT){
    	return FILE_QUIT;
	}
	
	return FILE_CONTINUE;
}

//-------------------HELPERS-------------------------
FILE_STATUS handleInputExe(char* str, const char* exeType)
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
	return FILE_CONTINUE;
}

FILE_STATUS handleOutputExe(char* str, const char* exeType)
{
	
   if (str[0] == '\0'){ // if string is empty give source file name with .out extension
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
		int i = outputChoice();
		
		if(i == 1){
			printf("You chose overwrite\n");
			backupOutputFile(str);
		}
		else if(i == 2){
			printf("You chose new output file\n");
			printf("\n---Please enter a valid new output file name: ");
			repromptFile(str, exeType, 2);
		}
		else if(i ==3){
			printf("You chose exit\n");
			return FILE_QUIT;
		}

		}
	strcpy(outputFileName, str);
	return FILE_CONTINUE;
}

int file_exists(const char* filename){
	FILE* file = fopen(filename, "r");  //  try opening it for reading
    if (file) {
        fclose(file);  //file exists and close it
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
            return 3;
        default:
            printf("Invalid choice, please try again.\n");
            outputChoice();
            break;
    }

return 0;
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
        printf("Error: Could not open output file: %s\n", outputFileName);
        return NULL;  
    }
    
    return file;
}

FILE* openListingFile(){
	FILE* file = fopen(listingFileName, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open listing file: %s\n", listingFileName);
        return NULL;  
    }
    
    return file;
}

FILE* openTempFile1(){
	FILE* file = fopen(tempFileName1, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 1: %s\n", tempFileName1);
        return NULL;  
    }
    
    return file;
}

FILE* openTempFile2(){
	FILE* file = fopen(tempFileName2, "w"); // open the file for reading
    if (file == NULL) {
        printf("Error: Could not open temp file 2: %s\n", tempFileName2);
        return NULL;  
    }
    
    return file;
}

void copyFileContents(){ // copies the conents of the input to all the other files
	
	int input;
	
	 
    while ((input = fgetc(inputFile)) != EOF) //read until EOF
    {
    	printf("READ: %c\n", input); 
        fputc(input, outputFile);
       fputc(input, listingFile);
       fputc(input, tempFile1);
       fputc(input, tempFile2);
       printf("copying...\n");
    }
	
}

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
}

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

void createTempFileNames(void)
{
    strcpy(tempFileName1, outputFileName);
    strcpy(tempFileName2, outputFileName);

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