/*---Instructions:---
	Three possible cases:
	1. No arguments only program name
	2. One argument - input file name
	3. Two arguments - input and output file names

	when extention not provided, default to .in for input and .out for output
	handle the cmd arguments

	Missing parameters:
	-prompt for missing 1 parameter
	-prompt for input name and output name
	-when 2 included no prompt

	Responses:
	-if extention included, use
	-if not not included, add default extentions

	**NOTE**: What if file is .txt
-------End Instructions-------*/

#include <stdio.h>
#include <stdlib.h>
#include "file_util.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	FILE *ifp = NULL;
	FILE *ofp = NULL;
	
	header();
	cmdArgs(argc, argv, &ifp, &ofp);
	
	return 0;
}