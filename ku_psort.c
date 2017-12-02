#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <limits.h>

#define NAME_POSIX "/my_mq"
#define MSG_SIZE 10


// Linked List
typedef struct _node{
	int value;
	struct _node* next;
}node;

typedef struct _LinkedList{
	int count;
	node* head;
	node* tail;
}list;

void init(list* lp){
	lp->count = 0;
	lp->head = NULL;
	lp->tail = NULL;
}

void addFirst(list* lp, int value){
	node* newNode = (node*)malloc(sizeof(node));
	newNode->value = value;
	newNode->next = NULL;
	if(lp->count == 0){
		lp->head = newNode;
		lp->tail = newNode;
	}
	else{
		newNode->next = lp->head;
		lp->head = newNode;
	}
	lp->count++;
//	printf("addFirst Done : %d\n",value);
}

void addLast(list* lp, int value){
	node* newNode = (node*)malloc(sizeof(node));
	newNode->value = value;
	newNode->next = NULL;
	if(lp->count == 0){
		lp->head = newNode;
		lp->tail = newNode;
	}
	else{
		lp->tail->next = newNode;
		lp->tail = newNode;
	}
	lp->count++;
}

int isEmpty(list* lp){
	if(lp->count == 0)return 1;
	return 0;
}

int removeFirst(list* lp){
	if(lp->count == 0) return -1;
	int value = lp->head->value;
	lp->head = lp->head->next;
	lp->count--;
	return value;
}

int peek(list* lp){
	if(lp->count == 0 )return -1;
	return lp->head->value;
}

// Radix Sort 0~9
void Sorting(int* sort_list, int len)
{
	int Radix, i, divi, minus_size;
	minus_size = 0;
	for(i=0;i<len;i++)
		if(sort_list[i]<0)
			minus_size++;

	for(Radix = 0;Radix<10;Radix++){
		// Radix sort init
		list** RS_list = (list**)malloc(sizeof(list*)*10);
		list** RS_list_minus = (list**)malloc(sizeof(list*)*10);
		for(i=0;i<10;i++){
			RS_list[i] = (list*)malloc(sizeof(list));
			RS_list_minus[i] = (list*)malloc(sizeof(list));
			init(RS_list[i]);
			init(RS_list_minus[i]);
		}
		divi =1;
		for(i=0;i<Radix;i++)divi*=10;

		// insert with Radix leaf
		for(i=0;i<len;i++){
			int num = sort_list[i];
			// printf("num: %d to insert Radix\n",num); //
			if(num>= 0)addLast(RS_list[(num/divi)%10],num);
			else addLast(RS_list_minus[(((-1)*num)/divi)%10],num);
		}
		if((RS_list[0]->count + RS_list_minus[0]->count) == len) break;
		// pop all and save
		int index = 0;
		for(i=9;i>=0;i--){
			while(!isEmpty(RS_list_minus[i])){
				sort_list[index] = removeFirst(RS_list_minus[i]);
				index++;
			}   
		}
		index = minus_size;
		for(i=0;i<10;i++){
			while(!isEmpty(RS_list[i])){
				sort_list[index] = removeFirst(RS_list[i]);
				index++;
			}
		}
		// free all
		for(i =0;i<10;i++) free(RS_list[i]);
		free(RS_list);
		for(i =0;i<10;i++) free(RS_list_minus[i]);
		free(RS_list_minus);
	}
}

int main(int argc,char* argv[])
{
	// init File
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	char* input_file = argv[3];
	char* output_file = argv[4];

	/*
	printf("M : %d\n",m);
	printf("N : %d\n", n);
	printf("Input File name is : %s\n", input_file);
	printf("Output File name is : %s \n", output_file);
	*/

	if(m<n){
		n = m;
	}

	// read input file
	FILE * In = fopen(input_file, "r");
	int input_buff;
	int div = (int)(((double)m) / ((double)n) + 0.5);
	
	// count
	int Process_count = 0;
	int * toSort_data = (int*) malloc (sizeof(int)*(m -div*(n-1)));
	pid_t * pid = (pid_t*)malloc(sizeof(pid_t) * n);
	int len = div;
	int read_index;

	pid[0] = fork();
	Process_count ++;
	
	// messege data
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = MSG_SIZE;
	mqd_t mqdes;

	if(pid[0] > 0){
//		printf("This is Root Parent \n");
		Process_count --;

		list **toMerge_data = (list **)malloc(sizeof(list*)*n);
		int i;
		int identity = Process_count;
		unsigned int prio;
		mqdes = mq_open(NAME_POSIX, O_CREAT | O_RDONLY, 0666, &attr);
		for(i=0;i<m;i++){
			int recData;
			 if(mq_receive(mqdes,(char*)&recData,MSG_SIZE, &prio) == -1)
				 perror("receive fail\n");
//			 printf("[mq: %d] receive [%d] : %d - current on queue : %ld\n",mqdes,Process_count,recData,attr.mq_curmsgs);
			 if(Process_count != identity){
				 identity = Process_count;
				 toMerge_data[Process_count - 1] = (list*)malloc(sizeof(list)*len);
				 init(toMerge_data[Process_count -1]);
			 }
			 addLast(toMerge_data[Process_count-1],recData);
		 }

//		 for(i=0;i<n;i++) printAll(toMerge_data[i]);
		FILE* Out = fopen(output_file,"w");
		int index_merge, merge_node;
		int isNone = 1;
		for(;;){
			merge_node = INT_MAX;
			isNone =1;
			for(i=0;i<n;i++){
				if(isEmpty(toMerge_data[i])) continue;
				if(peek(toMerge_data[i]) <= merge_node){
					isNone =0;
					merge_node = peek(toMerge_data[i]);
					index_merge = i;
				}
			}
			if(isNone) break;
//			printf("%d index remove : %d\n",index_merge,merge_node);
			fprintf(Out,"%d\n",removeFirst(toMerge_data[index_merge]));
		}
		fclose(Out);

		 // close messege queue
		 mq_close(mqdes);
		 mq_unlink(NAME_POSIX);
		 for(i=0;i<n;i++) free(toMerge_data[i]);
		 free(toMerge_data);
	}
	else if(pid[0] == 0){
		while(Process_count < n){
			printf("Process_count : %d [ ",Process_count);
			read_index = 0;
			while(read_index<div && fscanf(In,"%d",&input_buff) != EOF){
				toSort_data[read_index] = input_buff;
				printf("%d ",toSort_data[read_index]);
				read_index ++;
			}
			printf("]\n");
			pid[Process_count] = fork();
			if(pid[Process_count] > 0) {
				break;				
			}
			Process_count ++;
		}

		if(n == Process_count){
			read_index = 0;
			len = m - div*(n-1);
			printf("Process_count : %d [ ", Process_count);
			while(fscanf(In,"%d",&input_buff) != EOF){
				toSort_data[read_index] = input_buff;
				printf("%d ", toSort_data[read_index]);
				read_index++;
			}
			printf("]\n");
		}

		// sort each process
		Sorting(toSort_data, len);
		int i;
		printf("Process_count : %d #After sort# [ ",Process_count);
		for(i=0;i<len;i++)printf("%d ",toSort_data[i]);
		printf("]\n");

		
		unsigned int prio;
		// to Merge with Posix Message :: send
		mqdes = mq_open(NAME_POSIX,O_CREAT | O_WRONLY, 0666, &attr);
		i=0;
		while(1){
			int sendData;
			sendData = toSort_data[i];
//			printf("[mq: %d] send [%d] :%d - current on queue : %ld\n",mqdes,Process_count,sendData, attr.mq_curmsgs);
			if(mq_send(mqdes, (char*) &sendData,MSG_SIZE,prio) == -1)
				perror("send fail\n");
			i++;
			
			if(i == len)
				break;
		}
	}

	//close & free
	free(toSort_data);
	fclose(In);
	free(pid);
	return 0;
}

