#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char* argv[])
{
	// init File
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	char* input_file = argv[3];
	char* output_file = argv[4];

	printf("M : %d\n",m);
	printf("N : %d\n", n);
	printf("Input File name is : %s\n", input_file);
	printf("Output File name is : %s \n", output_file);

	// read input file
	FILE * In = fopen(input_file, "r");
	int input_buff;
	int div = (int)(m / n);
	
	// count
	int Process_count = 0;

	pid_t * pid = (pid_t*)malloc(sizeof(pid_t) * n);

	pid[0] = fork();
	Process_count ++;
	if(pid[0] > 0){
		printf("made Parent \n");
	}
	else if(pid[0] == 0){
		while(Process_count < n){
			printf("Process_count : %d\n",Process_count);
			int read_index = 0;
			while(fscanf(In,"%d",&input_buff) != EOF && read_index < div){
				printf("%d\n",input_buff);
				read_index ++;
			}
			pid[Process_count] = fork();
			if(pid[Process_count] > 0) break;
			Process_count ++;
		}
	}

	fclose(In);
	free(pid);
	return 0;
}
