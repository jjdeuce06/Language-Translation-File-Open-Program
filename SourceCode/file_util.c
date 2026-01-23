#include <stdio.h>
#include <string.h>
#define IN_EXTENSION ".in"


void cmdArgs(int argc, char *argv[], FILE* input_file, FILE* output_file)
{
    if (argc == 1)
    {
        printf("No command line parameters\n");
        noArgs(input_file, output_file);
    }
    else if (argc == 2)
    {
        printf("Input file: %s\n", argv[1]);
    }
    else
    {
        printf("Input file:  %s\n", argv[1]);
        printf("Output file: %s\n", argv[2]);
    }
}

void noArgs(FILE* input_file, FILE* output_file)
{
    char str[100];
    printf("Enter your input file name: ");
    scanf("%s", str);

    char* extension_indicator = strchr(str, ".");

    if (str != "")
    {
        if (extension_indicator < 0)
        {
            input_file = fopen(str, "r");
        }

        else
        {
            strcat(str, IN_EXTENSION);
            input_file = fopen(str, "r");
        }
    }
}
