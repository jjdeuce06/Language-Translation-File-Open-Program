#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	FILE *ifp, *ofp;
	
	//program name is always in argc, so there's at least one parameter
	if (argc == 1)
	{
		//call function to deal with no command line parameters
		printf("No command line parameters");
	}
	else if (argc == 2)
	{
		//call function to deal  with only an input file name
		printf("Only input file given");
	}
	else
	{
		//call function to deal with both input and output file name
		printf("Both parameters given");
	}
	
	
	
	return 0;
}