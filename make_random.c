#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char* argv[])
{
	// init File
	int m = atoi(argv[1]);
	char* output_file = argv[2];

	printf("M : %d\n",m);
	printf("Output File name is : %s \n", output_file);

	// write
	FILE * Out = fopen(output_file, "w");
	srand(time(NULL));
	int i = 0;
	for(;i<m;i++){
		int data;
		int isMinus;
		data = (int)(rand());
		isMinus = (int)(rand()%2);
		if(isMinus)data *=-1;
		fprintf(Out,"%d\n",data);
	}


	fclose(Out);
	return 0;
}
