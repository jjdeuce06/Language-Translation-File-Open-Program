#include <stdio.h>

int main(void)
{
	int		sum = 0, val;
	FILE	*ifp, *ofp;
	
	ifp = fopen("my_file", "r");	/* open for reading */
	ofp = fopen("outfile", "w"); /* open for writing */
	
	while (fscanf(ifp, "%d", &val) == 1)
		sum += val;
	fprintf(ofp, "The sum is %d.\n", sum);
	
	fclose(ifp);
	fclose(ofp);
	
	return 0;
}
