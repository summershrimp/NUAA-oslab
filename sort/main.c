#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int from;
    int count;
} Arg;

int *array;
int *merge_array;
int cmp(const void *a, const void *b)
{
    return *((int *)a) - *((int *)b);
}


void *map(void *args)
{
    Arg *arg = (Arg *)args;
    qsort(array + arg->from, arg->count, sizeof(int), cmp);
    return NULL;
}

void reduce(Arg* args)
{
    int p, *p1, *p2;
    p1 = array + args[0].from;
    p2 = array + args[1].from;
    p = 0;
    while(p1 != array + args[0].from + args[0].count && p2 != array + args[1].from + args[1].count)
    {
        *p1 < *p2 ? (merge_array[p++] = *(p1++)) : (merge_array[p++] = *(p2++));
    }
    while (p1 != array + args[0].from + args[0].count)
    {
        merge_array[p++] = *(p1++);
    }
   	while (p2 != array + args[1].from + args[1].count)
   	{
   		merge_array[p++] = *(p2++);
   	} 
}
int main(void)
{
    int cnt, i;
    Arg *arg_list;
    pthread_t *threads;
    scanf("%d", &cnt);
    array = (int *)malloc(sizeof(int) * cnt);
    merge_array = (int *)malloc(sizeof(int) * cnt);
    arg_list = (Arg *)malloc(sizeof(Arg) * 2);
    threads = (pthread_t *) malloc(sizeof(pthread_t) *2);
    for(i=0;i<cnt;++i)
        scanf("%d", array+i);
    arg_list[0].count = cnt/2;
    arg_list[0].from = 0;
    arg_list[1].count = cnt - arg_list[0].count;
    arg_list[1].from = arg_list[0].count;
    
    pthread_create(threads, NULL, map, arg_list);
    pthread_create(threads + 1, NULL, map, arg_list + 1);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    reduce(arg_list);
    for(i = 0; i< cnt; ++i)
        printf("%d ", merge_array[i]);
    
    return 0;
}
