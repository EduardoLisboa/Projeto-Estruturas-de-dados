#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

typedef struct node
{
	unsigned char value;
	unsigned short priority;
	struct node *next;
	struct node *left;
	struct node *right;
} Node;

typedef struct priority_queue
{
	Node *head;
} Queue;

typedef struct element
{
	unsigned short key;
	unsigned char byte[8];
	unsigned short size;
} Element;

typedef struct hash_table
{
	unsigned short size;
	Element *table[MAX];
} Hash_table;


Node* create_node(unsigned char value, unsigned short p, Node *next, Node *left, Node* right);
Queue* create_priority_queue();
void enqueue(Queue *pq, Node *node);
Node* dequeue(Queue *pq);
void print_queue(Queue *pq);
Node* create_tree(Node *left, Node *right);
void print_tree(Node *tree);

Hash_table* create_hash_table(Node *tree);
void put(Hash_table *ht, unsigned char key, unsigned char *byte, unsigned short size);
//unsigned short get(Hash_table *ht, unsigned short key);
void search(Hash_table *ht, Node *tree, unsigned char *byte, unsigned short size);

unsigned short get_trash_size (Hash_table *ht, unsigned char *freq);
unsigned short get_tree_size(Node *tree, unsigned short size);

void create_header(unsigned short trash, unsigned short tree);


int main(unsigned short argc, unsigned char const *argv[])
{
	if(argc != 2)
	{
		printf("Invalid number of arguments\n");
		exit(1);
	}
	
	FILE *p;
	p = fopen(argv[1], "rb");

	unsigned short i;
	unsigned char freq[256], ch;

	memset(freq, 0, 256);

	while(!feof(p))
	{
		ch = getc(p);
		freq[ch]++;
	}

	Queue *pq = create_priority_queue();

	for(i=0; i<255; i++)
		if(freq[i])
		{
			enqueue(pq, create_node(i, freq[i], NULL, NULL, NULL));
			printf("%c %d\n", i, freq[i]);
		}
	print_queue(pq);

	while(pq->head->next)
	{
		Node *n1 = dequeue(pq);
		Node *n2 = dequeue(pq);

		enqueue(pq, create_node('*', n1->priority+n2->priority, NULL, n1, n2));
		print_queue(pq);
	}
	printf("Tree:\n");
	print_tree(pq->head);
	printf("\n");

	Hash_table *ht = create_hash_table(pq->head);

	//printf("%u\n%u\n",trash_size(ht,freq),(8 - trash_size(ht,freq) % 8));
	
	unsigned short trash_size = get_trash_size(ht, freq);
	unsigned short tree_size = get_tree_size(pq->head, 0);

	FILE *out = fopen("compressed", "wb");
	create_header(trash_size, tree_size);

	fclose(p);
	return 0;
}

Node* create_node(unsigned char value, unsigned short p, Node *next, Node *left, Node* right)
{
	Node *new = malloc(sizeof(Node));
	new->value = value;
	new->priority = p;

	new->next = next;
	new->left = left;
	new->right = right;
}

Queue* create_priority_queue()
{
	Queue *new = malloc(sizeof(Queue));
	new->head = NULL;

	return new;
}

void enqueue(Queue *pq, Node *node)
{
	if(!pq->head || pq->head->priority >= node->priority)
	{
		node->next = pq->head;
		pq->head = node;
	}
	else
	{
		Node *n = pq->head;
		while(n->next && n->next->priority < node->priority)
			n = n->next;

		node->next = n->next;
		n->next = node;
	}
}

Node* dequeue(Queue *pq)
{
	if(!pq->head)
	{
		return NULL;
	}
	else
	{
		Node *removed = pq->head;
		pq->head = pq->head->next;

		return removed;
	}
}

void print_queue(Queue *pq)
{
	if(!pq->head)
	{
		printf("Empty queue\n");
		return;
	}

	Node *n = pq->head;
	while(n)
	{
		printf("%c ", n->value);
		n = n->next;
	}
	printf("\n");
}

Node* create_tree(Node *left, Node *right)
{
	Node *new = malloc(sizeof(Node));
	new->value = '*';
	new->priority = left->priority + right->priority;

	new->left = left;
	new->right = right;

	return new;
}

void print_tree(Node *tree)
{
	if(tree)
	{
		printf("%c ",tree->value);
		print_tree(tree->left);
		print_tree(tree->right);
	}
}

Hash_table* create_hash_table(Node *tree)
{
	Hash_table *ht = malloc(sizeof(Hash_table));

	unsigned short i;
	for(i=0; i<MAX; i++)
		ht->table[i] = NULL;

	unsigned char byte[8];
	search(ht, tree, byte, 0);
	return ht;
}

void search(Hash_table *ht, Node *tree, unsigned char *byte, unsigned short size)
{
	if(!tree)
		return;

	if(!tree->left && !tree->right)
	{
		byte[size] = '\0';
		put(ht, tree->value, byte, size);
	}
	else
	{
		byte[size] = '0';
		search(ht, tree->left, byte, size + 1);

		byte[size] = '1';
		search(ht, tree->right, byte, size + 1);
	}
}

void put(Hash_table *ht, unsigned char key, unsigned char *byte, unsigned short size)
{
	unsigned short index = key;

	printf("%c\n", key);
	printf("%u\n", index);
	printf("%s\n", byte);
	printf("%u\n\n", size);

	Element *new = malloc(sizeof(Element));
	ht->table[index] = new;

	new->key = key;
	strcpy(ht->table[index]->byte, byte);
	new->size = size;
}

unsigned short get_trash_size (Hash_table *ht, unsigned char *freq)
{
	unsigned short total, i;

	for(total=0, i=0; i<MAX-1; i++)
	{
		if(freq[i])
		{
			total += ht->table[i]->size*freq[i];
		}
	}
	return total;
}

unsigned short get_tree_size(Node *tree, unsigned short size)
{
	if(tree)
	{
		size++;
		size = get_tree_size(tree->left, size);
		size = get_tree_size(tree->right, size);
	}

	return size;
}

void create_header(unsigned short trash, unsigned short tree)
{
	unsigned char byte1, byte2;

	byte1 = 5 << trash;
}

/*
void put_in_file(const char *string, FILE *out)
{
    unsigned short i;

    unsigned char byte = 0;

    for(i = 0; i < 8; i++)
    {
        if(string[i] == '1')
        {
            byte |= 1 << (7-i);
        }
    }

    fprintf(out, "%c", byte);
}

unsigned short get(Hash_table *ht, unsigned short key)
{
	unsigned short h = key & MAX, start = h, stop = 0;

	while(start != h || !stop)
	{
		if(ht->table[h] && ht->table[h]->key == key)
			return ht->table[h]->value;

		h = (h + 1) % MAX;
		stop = 1;
	}

	return -1;
}
*/