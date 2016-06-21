#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int TH = 2;
int COUNT = 10000000;

typedef struct {
    int from;
    int count;
    int th;
} Arg;

double *calc_list; 
Arg *arg_list;

void *map(void *args)
{
    Arg *arg = (Arg*)args;
    int i, j;
    double cur;
    calc_list[arg->th] = 0;
    //printf("%d,%d,%d\n", arg->th, arg->from, arg->count);    
    for(i=0, j = arg->from; i<arg->count; ++i, ++j)
    {
        cur = 1.0/(2 * j + 1);
        if( j % 2 )
            cur = - cur;
        //printf("%f\n", cur);
        calc_list[arg->th] += cur; 
    }
    return NULL;
}

double reduce()
{
    int i=0;
    double ans = 0;
    for(;i < TH; ++i)
        ans += calc_list[i];
    return ans;
}

int main(void)
{
    pthread_t *threads;
    scanf("%d%d", &COUNT, &TH);
    

    calc_list = (double *)malloc(sizeof(double) * TH);
    arg_list = (Arg *)malloc(sizeof(Arg) * TH);
    threads = (pthread_t *)malloc(sizeof(pthread_t) * TH);
    int i = 0;
    int each_count = COUNT/TH;
    for(; i< TH; ++i)
    {
        arg_list[i].th = i;
        arg_list[i].from = i* each_count;
        arg_list[i].count = each_count;
        pthread_create(threads + i, NULL, map, arg_list + i);
    }

    for(i = 0; i < TH; ++i)
    {
        pthread_join(*(threads + i), NULL);
    }
    
    printf("%f", reduce() * 4.0);

    return 0;
}
