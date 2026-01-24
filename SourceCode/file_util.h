#ifndef FILE_UTIL_H
#define FILE_UTIL_H
    #include <stdio.h>


    void header();
    void outputMenu();
    void cmdArgs(int argc, char *argv[], FILE **input_file, FILE **output_file);
    void handleInputExe(char* str, FILE **input_file, const char* exeType);
    void handleOutputExe(char* str, FILE **output_file, const char* exeType);
    void repromptFile(char* str, FILE **file_ptr, const char* exeType, int choice);

    void noArgs(FILE** input_file, FILE** output_file);
    void oneArg(char *argv[], FILE **output_file);
    void twoArg(char *argv[], FILE **input_file, FILE **output_file);

    void outputChoice();

    
#endif