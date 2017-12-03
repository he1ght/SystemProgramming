#include <stdio.h>
#include <limits.h>

int main(int argc,char* argv[])
{
	// init File
	char* file = argv[1];
	FILE * f = fopen(file,"r");
	int num = INT_MIN;
	int next;
	int count = 0;
	while(fscanf(f,"%d",&next) != EOF){
		if(num <= next){
			num = next;
			count++;
		}
		else{
			printf("I counted until: %d... And Test Fail!\n",count);
			return 0;
		}
	}
	printf("I counted until: %d... And Test Pass!\n",count);
}
