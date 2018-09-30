#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 256

typedef struct node
{
	unsigned char value;
	int priority;
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
	int key;
	unsigned char byte[8];
	int size;
} Element;

typedef struct hash_table
{
	int size;
	Element *table[MAX];
} Hash_table;


Node* create_node(unsigned char value, int p, Node *next, Node *left, Node* right);
Queue* create_priority_queue();
void enqueue(Queue *pq, Node *node);
Node* dequeue(Queue *pq);
void print_queue(Queue *pq);
Node* create_tree(Node *left, Node *right);
void print_tree(Node *tree);

Hash_table* create_hash_table(Node *tree);
void put(Hash_table *ht, unsigned char key, unsigned char *byte, int size);
unsigned char* get(Hash_table *ht, unsigned char key);
void search(Hash_table *ht, Node *tree, unsigned char *byte, int size);

int file_data_size(Hash_table *ht, unsigned char *freq);
int get_trash_size (int size);
int get_tree_size(Node *tree, int size);

void create_header(FILE *out, int trash, int tree);
void write_tree(Node *tree, FILE *out);
void compress(FILE *input_file, FILE *out, Hash_table *ht, int size);
void write_byte(FILE *out, unsigned char* byte_str);
unsigned char set_bit(unsigned char c, int i);


int main(int argc, unsigned char const *argv[])
{
	if(argc != 2)
	{
		printf("Invalid number of arguments\n");
		exit(1);
	}
	
	FILE *input_file;
	input_file = fopen(argv[1], "rb");

	int i;
	unsigned char freq[MAX], ch;

	memset(freq, 0, MAX);

	fread(&ch, sizeof(ch), 1, input_file);
	while(!feof(input_file))
	{
		freq[ch]++;
		fread(&ch, sizeof(ch), 1, input_file);
	}

	Queue *pq = create_priority_queue();

	for(i=0; i<MAX; i++)
		if(freq[i])
		{
			enqueue(pq, create_node(i, freq[i], NULL, NULL, NULL));
		}

	while(pq->head->next)
	{
		Node *n1 = dequeue(pq);
		Node *n2 = dequeue(pq);
		enqueue(pq, create_node('*', n1->priority+n2->priority, NULL, n1, n2));
	}

	Hash_table *ht = create_hash_table(pq->head);

	int trash_size = get_trash_size(file_data_size(ht, freq));
	int tree_size = get_tree_size(pq->head, 0);

	FILE *out = fopen("256.huff", "wb");
	create_header(out, trash_size, tree_size);
	write_tree(pq->head, out);
	compress(input_file, out, ht, file_data_size(ht, freq));

	fclose(input_file);
	fclose(out);
	return 0;
}

Node* create_node(unsigned char value, int p, Node *next, Node *left, Node* right)
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

	int i;
	for(i=0; i<MAX; i++)
		ht->table[i] = NULL;

	unsigned char byte[8];
	search(ht, tree, byte, 0);
	return ht;
}

void put(Hash_table *ht, unsigned char key, unsigned char *byte, int size)
{
	int index = key;

	Element *new = malloc(sizeof(Element));
	ht->table[index] = new;

	new->key = key;
	strcpy(ht->table[index]->byte, byte);
	new->size = size;
}

unsigned char* get(Hash_table *ht, unsigned char key)
{
	int h = key % MAX;

	if(ht->table[h])
		return ht->table[h]->byte;
}

void search(Hash_table *ht, Node *tree, unsigned char *byte, int size)
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

int file_data_size(Hash_table *ht, unsigned char *freq)
{
	int total, i;

	for(total=0, i=0; i<MAX; i++)
	{
		if(freq[i])
		{
			total += ht->table[i]->size*freq[i];
		}
	}

	return total; 
}

int get_trash_size (int size)
{
	return 8 - (size % 8);
}

int get_tree_size(Node *tree, int size)
{
	if(tree)
	{
		size++;
		size = get_tree_size(tree->left, size);
		size = get_tree_size(tree->right, size);
	}

	return size;
}

void create_header(FILE *out, int trash, int tree)
{
	unsigned char byte1, byte2, aux = tree >> 8;
	byte1 = trash << 5;
	byte1 = byte1 | (tree >> 8);
	byte2 = (tree << 8) >> 8;

	fprintf(out, "%c", byte1);
	fprintf(out, "%c", byte2);
}

void write_tree(Node *tree, FILE *out)
{
	if(tree)
	{
		fprintf(out, "%c", tree->value);
		write_tree(tree->left, out);
		write_tree(tree->right, out);
	}
}

void compress(FILE *input_file, FILE *out, Hash_table *ht, int size)
{
	rewind(input_file);

	int byte_count = 0, new_byte_size = 0, i, j, there_is_rest = 0;
	unsigned char byte[8] = "\0", temp[8] = "\0", rest[8] = "\0", ch;

	while(byte_count < size/8)
	{
		if(there_is_rest)
		{
			there_is_rest = 0;
			strcpy(temp, rest);
			memset(rest, '\0', 8);
		}
		else
		{
			fread(&ch, sizeof(ch), 1, input_file);
			strcpy(temp, get(ht, ch));
		}

		new_byte_size = strlen(temp) + strlen(byte);

		if(new_byte_size == 8)
		{
			strcat(byte, temp);
			write_byte(out, byte);
			memset(byte, '\0', 8);
			byte_count++;
		}
		else if(new_byte_size > 8)
		{
			there_is_rest = 1;
			for(i=8-strlen(byte), j=0; i<strlen(temp)+1; i++, j++)
			{
					rest[j] = temp[i];
			}
			temp[8-strlen(byte)] = '\0';

			strcat(byte, temp);
			write_byte(out, byte);
			memset(byte, 0, 8);
			byte_count++;
		}
		else
		{
			strcat(byte, temp);
		}
	}
	if(there_is_rest)
	{
		strcat(byte, rest);
		for(i=strlen(rest)%8; i<8; i++)
			rest[i] = '0';
		rest[8] = '\0';
		write_byte(out, rest);
	}
}

void write_byte(FILE *out, unsigned char* byte_str)
{
	int i;
	unsigned char byte = 0;
	
	for(i=0; i<8; i++)
	{
		if(byte_str[i] == '1')
		{
			byte = set_bit(byte, (8-i)-1);
		}
	}
	fprintf(out, "%c", byte);
}

unsigned char set_bit(unsigned char c, int i)
{
	unsigned char mask = 1 << i;
	return mask | c;
}
