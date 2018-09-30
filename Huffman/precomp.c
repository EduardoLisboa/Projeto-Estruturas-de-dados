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
unsigned char* get(Hash_table *ht, unsigned char key);
void search(Hash_table *ht, Node *tree, unsigned char *byte, unsigned short size);

unsigned short file_data_size(Hash_table *ht, unsigned char *freq);
unsigned short get_trash_size (unsigned short size);
unsigned short get_tree_size(Node *tree, unsigned short size);

void create_header(FILE *out, unsigned short trash, unsigned short tree);
void write_tree(Node *tree, FILE *out);
void compress(FILE *input_file, FILE *out, Hash_table *ht, unsigned short size);
void write_byte(FILE *out, unsigned char* byte_str);


int main(unsigned short argc, unsigned char const *argv[])
{
	if(argc != 2)
	{
		printf("Invalid number of arguments\n");
		exit(1);
	}
	
	FILE *input_file;
	input_file = fopen(argv[1], "rb");

	unsigned short i;
	unsigned char freq[MAX], ch;

	memset(freq, 0, MAX);

	while(!feof(input_file))
	{
		ch = getc(input_file);
		freq[ch]++;
	}

	Queue *pq = create_priority_queue();

	for(i=0; i<MAX-1; i++)
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
	
	unsigned short trash_size = get_trash_size(file_data_size(ht, freq));
	unsigned short tree_size = get_tree_size(pq->head, 0);

	FILE *out = fopen("compressed.huff", "wb");
	create_header(out, trash_size, tree_size);
	write_tree(pq->head, out);
	compress(input_file, out, ht, file_data_size(ht, freq));

	fclose(input_file);
	fclose(out);
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

unsigned char* get(Hash_table *ht, unsigned char key)
{
	unsigned short h = key % MAX;
	printf("catado == %s\n", ht->table[h]->byte);

	if(ht->table[h])
		return ht->table[h]->byte;

	printf("Erro get\n");
	exit(1);
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

unsigned short file_data_size(Hash_table *ht, unsigned char *freq)
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

unsigned short get_trash_size (unsigned short size)
{
	return 8 - (size % 8);
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

void create_header(FILE *out, unsigned short trash, unsigned short tree)
{
	//printf("trash %hu\ntree %hu\n", trash, tree);
	unsigned char byte1, byte2, aux = tree >> 8;
	byte1 = trash << 5;
	byte1 = byte1 | (tree >> 8);
	byte2 = (tree << 8) >> 8;

	printf("\n%hu\n", byte1);
	printf("%hu\n", byte2);
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

void compress(FILE *input_file, FILE *out, Hash_table *ht, unsigned short size)
{
	rewind(input_file);

	unsigned short byte_count = 0, new_byte_size = 0, i, j, there_is_rest = 0;
	unsigned char byte[8] = "\0", temp[8] = "\0", rest[8] = "\0", ch;

	while(byte_count < size/8)
	{
		printf("lp infinito? %hu\n", byte_count);
		if(there_is_rest)
		{
			there_is_rest = 0;
			printf("rest\n");
			strcpy(temp, rest);
			memset(rest, '\0', 8);
		}
		else
		{
			printf("else\n");
			ch = getc(input_file);
			printf("lido == %c\n", ch);
			strcpy(temp, get(ht, ch));
		}

		new_byte_size = strlen(temp) + strlen(byte);
		printf("new_byte_size == %hu\n", new_byte_size);

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
			printf("resto %s\n", rest);
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
		printf("last byte %lu\n", strlen(rest));
		printf("rest = %s\n", rest);
		strcat(byte, rest);
		for(i=strlen(rest)%8; i<8; i++)
			rest[i] = '0';
		rest[8] = '\0';
		write_byte(out, rest);
	}
}

void write_byte(FILE *out, unsigned char* byte_str)
{
	printf("byte str = %s\n", byte_str);
	unsigned short i;
	unsigned char byte = 0;
	
	for(i=0; i<8; i++)
	{
		if(byte_str[i] == '0')
		{
			byte << 1;
		}
		else if(byte_str[i] == '1')
		{
			byte = 1 | (byte << 1);
		}
	}
	printf("byte!!!! %hu\n", byte);
	fprintf(out, "%c", byte);
}

void write_last_byte(FILE *out, unsigned char* byte_str)
{

}

/*
void compress(FILE *input_file, FILE *out, Hash_table *ht, unsigned short size)
{
	rewind(input_file);
	printf("size = %hu\n", size/8);

	unsigned short byte_count = 0;
	unsigned char rest[8];

	while(byte_count < size/8)
	{
		strcpy(rest, select_byte(input_file, out, ht,rest));
		if(rest)
	}
}

unsigned char* select_byte(FILE *input_file, FILE *out, Hash_table *ht, unsigned char *rest)
{
	unsigned char byte[8], temp[8], ch;
	unsigned short new_size = 0, i, j;

	while(strlen(byte) != 8)
	{
		ch = getc(input_file);
		strcpy(temp, get(ht, ch));
		new_size = (strlen(temp) + strlen(byte));

		if(new_size > 8)
		{
			for(i=new_size-8, j=0; i>strlen(temp)+1; i++, j++)
			{
				rest[j] = temp[i];
			}
			//strcpy(rest, temp[new_size-8];
			temp[new_size-8] = '\0';
			strcpy(byte, temp);
			write_byte(input_file, byte);
			return rest;
		}
	}
	write_byte(input_file, byte);
	return NULL;
}

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
*/