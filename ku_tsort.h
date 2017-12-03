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

// List struct include len
typedef struct _arr{
	size_t len;
	int * array;
}arr;

// Linked List function
void init(list* lp);
void addFirst(list* lp, int value);
void addLast(list* lp, int value);
int isEmpty(list* lp);
int removeFirst(list* lp);
int peek(list* lp);

// Radix Sort
void Sorting(int* sort_list, int len);

// Thread
void* Part_Sort(void* data);
