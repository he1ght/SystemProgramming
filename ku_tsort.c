#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "ku_tsort.h"

//pthread_mutex_t mutex;

// Linked List function
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

// Thread to Sort
void* Part_Sort(void* Rdata)
{
	arr data = *((arr *)Rdata);
	int i;

	// print before Sort
	/*
	printf("Thread [%ld] : [ ",pthread_self());
	for(i=0;i<data.len;i++)
		printf("%d ",data.array[i]);
	printf("]\n");
	*/

	Sorting(data.array, data.len);
	// print after Sort
	/*
	printf("Thread #After Sort# [%ld] : [ ",pthread_self());
	for(i=0;i<data.len;i++)
		printf("%d ",data.array[i]);
	printf("]\n");
	*/

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
	int Thread_count = 0;
	arr* toSort_data;
	toSort_data = (arr*) malloc (sizeof(arr)*n);

	// init to Sort Data & input
	int i;
	for(i=0;i<n-1;i++) {
		toSort_data[i].array = (int*) malloc (sizeof(int)*div);
		toSort_data[i].len = 0;
	}
	toSort_data[n-1].array = (int*) malloc (sizeof(int)*(m-div*(n-1)));
	toSort_data[n-1].len = 0;

	for(i=0;i<n-1;i++){
		int input_buff, read_index = 0;
		while(read_index < div && fscanf(In,"%d",&input_buff) != EOF){
			toSort_data[i].array[read_index]=input_buff;
			read_index ++;
			toSort_data[i].len++;
		}
	}
	for(i=0;i<(m-div*(n-1));i++){
		int input_buff;
		if(fscanf(In,"%d",&input_buff) == EOF) break;
		toSort_data[n-1].array[i] = input_buff;
		toSort_data[n-1].len++;
	}
	fclose(In);

	// print Input Data
/*
	printf("Input Data \n"); 
	for(i=0;i<n;i++){
		int j;
//		printf("[ %d: len: %d\n",i,(int)toSort_data[i].len);
		printf("[ %d: ",i); 
		for(j=0;j<toSort_data[i].len;j++){
			printf("%d ",toSort_data[i].array[j]);
		}
		printf("] \n"); 
	}
*/

	// give data to each Thread
	int thr_id, status;
	pthread_t * p_thread = (pthread_t *) malloc(sizeof(pthread_t)*n);
	for(i=0;i<n;i++){
		thr_id = pthread_create(&p_thread[i],NULL,Part_Sort,(void*)&toSort_data[i]);
//		printf("i: %d, thr_id: %d, p_thread[i]: %ld\n",i,thr_id,p_thread[i]); //
		if(thr_id < 0){
			perror("Thread create error: ");
			exit(0);
		}
	}

	Thread_count = n;
	// Why n convert to 0 here?
	for(i=0;i<Thread_count;i++){
		pthread_join(p_thread[i], (void **)&status);
//		printf("return thread %d: %d\n",i,status);
	}
	n = Thread_count;

	// print After Each Tread Sort
/*
	printf("After Each Thread Sort Data \n");
//	printf("n: %d\n", n);
	for(i=0;i<n;i++){
		printf("[ %d: ",i); 
		int j;
		for(j=0;j<toSort_data[i].len;j++){
			printf("%d ",toSort_data[i].array[j]);
		}
		printf("] \n"); 
	}
*/

	// merge sort
	FILE * Out = fopen(output_file,"w");
	int *each_index = (int*)malloc(sizeof(int)*n);
	for(i=0;i<n;i++) each_index[i] = 0;
	for(;;){
		int j;
		int index_node = -1, min_node = INT_MAX;
		for(j=0;j<n;j++){
			if(each_index[j] == toSort_data[j].len) continue;
			if(toSort_data[j].array[each_index[j]] <= min_node){
				index_node = j;
				min_node = toSort_data[j].array[each_index[j]];
			}
		}
		// printf("index: %d, min: %d\n",index_node,min_node);
		if(index_node == -1) break;
		fprintf(Out,"%d\n",min_node);
		each_index[index_node]++;
	}
	fclose(Out);

	// Print Done
	printf("Entire Sorting Process Done...\n");

	// free 
	free(each_index);
	for(i=0;i<n;i++) free(toSort_data[i].array);
	free(toSort_data);
	free(p_thread);
	return 0;
}

