#include <stdio.h>

#define MAXSTRING 100

int main(void)
{
	char file_name[MAXSTRING];
	int c;
	long file_len;
	FILE *ifp;
	
	fprintf(stderr, "\nInput a file name: ");
	scanf("%s", file_name);
	ifp = fopen(file_name, "rb");	//b for binary in windows
	
	fseek(ifp, 0, SEEK_END);		//  set to end of file
	fseek(ifp, -1, SEEK_CUR);		//  (back one from end)
	file_len = ftell(ifp);    		//  get position at end, or length of file
	while (file_len >= 0) {          //  while our position is not before beginning
		c = fgetc(ifp);				//  get a character from the file (forward one)
		fputc(c, stdout);			//  put char to screen
		fseek(ifp, -2, SEEK_CUR);   //  set to next char back in file. (back two)
		file_len--;                  //  we are now back by one character.
	}
	
	return 0;
}

