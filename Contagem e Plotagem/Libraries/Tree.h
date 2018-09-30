#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Códigos comuns às duas estruturas

typedef struct tree
{
    long int value;
    int h;
    struct tree *left;
    struct tree *right;
}Tree;

bool is_empty(Tree *bt)
{
    return (bt == NULL);
}

Tree* create_empty_tree()
{
    return NULL;
}

Tree* create_tree(int value, Tree *left, Tree *right)
{
    Tree *new_tree = (Tree*)malloc(sizeof(Tree));
    new_tree->value = value;
    new_tree->left = left;
    new_tree->right = right;
    new_tree->h = 1;

    return new_tree;
}

void print_pre_order(Tree *bt)
{
    if(!is_empty(bt))
    {
        printf("%d ", bt->value);
        print_pre_order(bt->left);
        print_pre_order(bt->right);
    }
}

//*********************************************************************//
//******************@@@@@@@@@**@@@@@@@@****@@@@@@@@********************//
//******************@@     @@**@@      @@**@@      @@******************//
//******************@@     @@**@@      @@**@@      @@******************//
//******************@@@@@@@@@**@@@@@@@@****@@@@@@@@********************//
//******************@@     @@**@@      @@**@@      @@******************//
//******************@@     @@**@@      @@**@@      @@******************//
//******************@@     @@**@@@@@@@@****@@@@@@@@********************//
//*********************************************************************//

// Códigos específicos da árvore de busca binária

Tree* add_in_abb(Tree *bt, int value)
{
    if(bt == NULL) bt = create_tree(value, NULL, NULL);
    else if(bt->value > value) bt->left = add_in_abb(bt->left, value);
    else bt->right = add_in_abb(bt->right, value);

    return bt;
}

Tree *search_abb(FILE *file, Tree *bt, long int value, int cont)
{
    if (bt == NULL)
    {
        cont++;
        fprintf(file, "%li\t%d\t", value, cont);
        return bt;
    }

    if (bt->value == value)
    {
        cont++;
        fprintf(file, "%li\t%d\t", value, cont);
        return bt;
    }
    else if (bt->value > value) return search_abb(file, bt->left, value, cont + 1);
    else return search_abb(file, bt->right, value, cont + 1);

    return bt;
}

//*********************************************************************//
//******************@@@@@@@@@**@@       @@**@@*************************//
//******************@@     @@**@@       @@**@@*************************//
//******************@@     @@***@@     @@***@@*************************//
//******************@@@@@@@@@***@@     @@***@@*************************//
//******************@@     @@****@@   @@****@@*************************//
//******************@@     @@*****@@ @@*****@@*************************//
//******************@@     @@******@@@******@@@@@@@@@******************//
//*********************************************************************//

//Códigos específicos da AVL

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int h(Tree *bt)
{
    if(bt == NULL) return -1;
    else return 1 + max(h(bt->left), h(bt->right));
}

int is_balanced(Tree *bt)
{
    int bf = h(bt->left) - h(bt->right);
    return ((-1 <= bf) && (bf <= 1));
}

int check_if_balanced(Tree *bt)
{
    if (bt == NULL)
        return 0;
    return h(bt->left) - h(bt->right);
}

Tree* rotate_left(Tree *bt)
{
    Tree *subtree_root = NULL;

    if(bt != NULL && bt->right != NULL)
    {
        subtree_root = bt->right;
        bt->right = subtree_root->left;
        subtree_root->left = bt;
    }

    subtree_root->h = h(subtree_root);
    bt->h = h(bt);

    return subtree_root;
}

Tree* rotate_right(Tree *bt)
{
    Tree *subtree_root = NULL;

    if(bt != NULL && bt->left != NULL)
    {
        subtree_root = bt->left;
        bt->left = subtree_root->right;
        subtree_root->right = bt;
    }

    subtree_root->h = h(subtree_root);
    bt->h = h(bt);
    
    return subtree_root;
}

Tree* add_in_avl(Tree *bt, int value)
{
    if(bt == NULL) return create_tree(value, NULL, NULL);
    else if(bt->value > value) bt->left = add_in_avl(bt->left, value);
    else bt->right = add_in_avl(bt->right, value);

    bt->h = h(bt);
    Tree *child;

    if(check_if_balanced(bt) == 2 || check_if_balanced(bt) == -2)
    {
        if(check_if_balanced(bt) == 2)
        {
            child = bt->left;
            if(check_if_balanced(child) == -1)
            {
                bt->left = rotate_left(child);
            }
            bt = rotate_right(bt);
        }
        else if(check_if_balanced(bt) == -2)
        {
            child = bt->right;
            if(check_if_balanced(child) == 1)
            {
                bt->right = rotate_right(child);
            }
            bt = rotate_left(bt);
        }
    }

    return bt;
}

Tree *search_avl(FILE *file, Tree *bt, long int value, int cont)
{
    if (bt == NULL)
    {
        if(cont == 0) cont++;
        fprintf(file, "%d\n", cont);
        return bt;
    }

    if (bt->value == value)
    {
        if(cont == 0) cont++;
        fprintf(file, "%d\n", cont);
        return bt;
    }
    else if (bt->value > value) return search_avl(file, bt->left, value, cont + 1);
    else return search_avl(file, bt->right, value, cont + 1);

    return bt;
}