#include <stdio.h>
#include <stdlib.h>

void insertion(int *v, int size)
{
    int i, j, key;
    for(i = 1; i < size; i++)
    {
        key = v[i];
        j = i - 1;

        while((j >= 0) && (v[j] > key))
        {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = key;
    }
}