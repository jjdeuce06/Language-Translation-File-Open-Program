#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	FILE *ifp, *ofp;
	
	//handle the cmd arguments
	cmdArgs(argc, argv, *ifp, *ofp);
	

	return 0;
}