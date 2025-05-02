#include "Sort.h"
#include "Utils.h"

int partition(long* vector, int left, int right)
{
    long pivot = vector[left];
    int i = left + 1;
    int j = right + 1;

    for (j = left + 1; j <= right; j++)
    {
        if (vector[j] <= pivot)
        {
            interchange_values((vector + i), (vector + j));
            i++;
        }

    }
    interchange_values((vector + left), (vector + i - 1));
    return (i - 1);
}

void quick_sort(long* vector, int left, int right) {
    int pivot_position = 0;


    if (left < right)
    {
        pivot_position = partition(vector, left, right);
        quick_sort(vector, left, pivot_position - 1);
        quick_sort(vector, pivot_position + 1, right);
    }


}