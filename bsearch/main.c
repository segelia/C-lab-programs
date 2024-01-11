#include <stddef.h> // size_t
#include <stdio.h>
#include <string.h> // strcmp
#include <stdlib.h>

void* my_bsearch(const void* key, const void* base, size_t num, size_t size,
                 int (*compar)(const void*, const void*))
{
    const char* array = (const char*)base;

    size_t left = 0;
    size_t right = num - 1;

    while (left <= right)
    {
        size_t mid = (left + right) / 2;

        const char* element = array + (mid * size);
        int cmp = compar(key, element);

        if (cmp == 0)
        {
            return (void*)element;
        }
        else if (cmp < 0)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }

    return NULL;
}

int compare_int(const void* a, const void* b)
{
    int num_a = *(const int*)a;
    int num_b = *(const int*)b;

    return num_a - num_b;
}

int compare_float(const void* a, const void* b)
{
    float num_a = *(const float*)a;
    float num_b = *(const float*)b;
    float delta = 0.00001;

    if (num_a < num_b - delta)
        return -1;
    else if (num_a > num_b + delta)
        return 1;
    else
        return 0;
}

int compare_string(const void* a, const void* b)
{
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;

    return strcmp(str_a, str_b);
}

void test_bsearch()
{
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int size = sizeof(array) / sizeof(int);
    int key = 5;

    int* result = my_bsearch(&key, array, size, sizeof(int), &compare_int);

    if (result != NULL)
    {
        printf("Key found at index: %ld\n", result - array);
    }
    else
    {
        printf("Key not found\n");
    }
}

void test_bsearch_float()
{
    float array[] = {1.0, 2.5, 3.2, 4.7, 5.9};
    int size = sizeof(array) / sizeof(float);
    float key = 4.7;

    float* result = my_bsearch(&key, array, size, sizeof(float), &compare_float);

    if (result != NULL)
    {
        printf("Key found at index: %ld\n", result - array);
    }
    else
    {
        printf("Key not found\n");
    }
}

void test_bsearch_string()
{
    const char* array[] = {"julka", "magda", "piotrek", "lukasz"};
    int size = sizeof(array) / sizeof(const char*);
    const char* key = "piotrek";

    const char** result = my_bsearch(&key, array, size, sizeof(const char*), compare_string);

    if (result != NULL)
    {
        printf("Key found at index: %ld\n", result - array);
    }
    else
    {
        printf("Key not found\n");
    }
}

int main()
{
    test_bsearch();
    test_bsearch_float();
    test_bsearch_string();

    return 0;
}
