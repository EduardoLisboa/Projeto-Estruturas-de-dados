#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Libraries/Tree.h"
#include "Libraries/InsertionSort.h"
#define MAX_SIZE 10000

int main()
{
    srand(time(NULL));

    FILE *file;
    file = fopen("ABBvsAVL.txt", "w+");

    Tree *ABB = create_empty_tree();
    Tree *AVL = create_empty_tree();
    long int i = 0, j = 0, k = 5;

    printf("1 - Worst ABB case\n2 - Random values\n");
    int option;
    for(;;)
    {
        scanf("%d", &option);

        if(option != 1 && option != 2)
        {
            printf("Select a valid option\n");
        }
        else
        {
            printf("\nAdding...\n");
            if(option == 1)
            {
                while (i < MAX_SIZE)
                {
                    if(i == j)
                    {
                        printf("%d%%\n", k);
                        k += 5;
                        j += 5 * (MAX_SIZE / 100);
                    }
                    AVL = add_in_avl(AVL, i);
                    ABB = add_in_abb(ABB, i);
                    i++;
                }
                printf("Addition complete!\n\n");
                break;
            }
            else if(option == 2)
            {
                while (i < MAX_SIZE)
                {
                    if(i == j)
                    {
                        printf("%d%%\n", k);
                        k += 5;
                        j += 5 * (MAX_SIZE / 100);
                    }
                    long int j = rand()%MAX_SIZE;
                    AVL = add_in_avl(AVL, i);                
                    ABB = add_in_abb(ABB, j);
                    i++;
                }
                printf("Addition complete!\n\n");
                break;
            }
        }
        
    }

    int vector[MAX_SIZE + 1];
    i = 0;
    while (i < MAX_SIZE)
    {
        vector[i] = rand()%MAX_SIZE;
        i++;
    }
    i = 0;

    insertion(vector, MAX_SIZE + 1);

    fprintf(file, "VALOR\tABB\tAVL\n");

    printf("Searching...\n");
    j = 0, k = 5;
    while (i < MAX_SIZE)
    {
        if(i == j)
        {
            printf("%d%%\n", k);
            k += 5;
            j += 5 * (MAX_SIZE / 100);
        }
        search_abb(file, ABB, vector[i], 0);
        search_avl(file, AVL, vector[i], 0);
        i++;
    }
    printf("Search complete!\n");

    fclose(file);
    return 0;
}