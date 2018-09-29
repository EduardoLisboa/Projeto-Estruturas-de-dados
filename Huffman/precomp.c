#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

typedef struct node
{
	unsigned char value;
	unsigned priority;
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
	unsigned key;
	unsigned char byte[8];
	unsigned size;
} Element;

typedef struct hash_table
{
	unsigned size;
	Element *table[MAX];
} Hash_table;


Node* create_node(unsigned char value, unsigned p, Node *next, Node *left, Node* right);
Queue* create_priority_queue();
void enqueue(Queue *pq, Node *node);
Node* dequeue(Queue *pq);
void print_queue(Queue *pq);
Node* create_tree(Node *left, Node *right);
void print_tree(Node *tree);

Hash_table* create_hash_table(Node *tree);
void put(Hash_table *ht, unsigned char key, unsigned char *byte, unsigned size);
//unsigned get(Hash_table *ht, unsigned key);
void search(Hash_table *ht, Node *tree, unsigned char *byte, unsigned size);

void create_header(Node *tree, unsigned tree_size);
unsigned short tree_nodes(Node *tree, unsigned short size);
void print_bin();

void trash_size (Hash_table *ht, unsigned char *freq)
void tree_to_bin(Node *bt)
void put_in_file(const char *string, FILE *out)


int main(unsigned argc, unsigned char const *argv[])
{
	if(argc != 2)
	{
		printf("Invalid number of arguments\n");
		exit(1);
	}
	
	FILE *p;
	p = fopen(argv[1], "rb");

	unsigned i;
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

	short unsigned s = tree_nodes(pq->head, 0);

	//printf("%u\n%u\n",trash_size(ht,freq),(8 - trash_size(ht,freq) % 8));
	FILE *out = fopen("compressed", "wb");
	
	trash_size(ht, freq);

	tree_to_bin(pq->head);

	fclose(p);
	return 0;
}

Node* create_node(unsigned char value, unsigned p, Node *next, Node *left, Node* right)
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

	unsigned i;
	for(i=0; i<MAX; i++)
		ht->table[i] = NULL;

	unsigned char byte[8];
	search(ht, tree, byte, 0);
	return ht;
}

void search(Hash_table *ht, Node *tree, unsigned char *byte, unsigned size)
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

void put(Hash_table *ht, unsigned char key, unsigned char *byte, unsigned size)
{
	unsigned index = key;

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

unsigned short tree_nodes(Node *tree, unsigned short size)
{
	if(tree)
	{
		size++;
		size = tree_nodes(tree->left, size);
		size = tree_nodes(tree->right, size);
	}

	return size;
}

void trash_size (Hash_table *ht, unsigned char *freq)
{
	unsigned total = 0;
	unsigned i;

	for(i = 0; i < MAX - 1; i++)
	{
		if(freq[i] != 0)
		{
			total += ht->table[i]->size * freq[i];
		}
	}
	
	unsigned trash = 8 - (total % 8);
	unsigned char aux[4];
	itoa(trash, aux, 2);

	unsigned char bin[4];
	memset(bin, '0', 4);

	unsigned diff = 3 - strlen(aux);

	unsigned j = 0;

	if(diff)
	{
		for(i = diff; i < strlen(bin); i++, j++)
		{
			bin[i] = aux[j];
		}
		printf("%s\n", bin);
	}
	else printf("%s\n", aux);
}

void tree_to_bin(Node *bt)
{
	unsigned tree_size = tree_nodes(bt, 0);
	unsigned char aux[14];
	itoa(tree_size, aux, 2);

	unsigned char bin[14];
	memset(bin, '0', 14);

	unsigned diff = 13 - strlen(aux);

	unsigned i, j = 0;

	if(diff)
	{
		for(i = diff; i < strlen(bin); i++, j++)
		{
			bin[i] = aux[j];
		}
		printf("%s\n", bin);
	}
	else printf("%s\n", aux);
}

void put_in_file(const char *string, FILE *out)
{
    int i;

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

/*
void create_header(Node *tree, unsigned tree_size)
{
	unsigned char byte1, byte2;

	byte1 = byte1 << 5;
}

unsigned get(Hash_table *ht, unsigned key)
{
	unsigned h = key & MAX, start = h, stop = 0;

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