#include <stdio.h>


void cmdArgs(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("No command line parameters\n");
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
