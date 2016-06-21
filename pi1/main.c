#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define TH 2
#define COUNT 10000000

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
    pthread_t thread;
    calc_list = (double *)malloc(sizeof(double) * TH);
    arg_list = (Arg *)malloc(sizeof(Arg) * TH);

    memset(calc_list, 0, sizeof(double) * TH);
    arg_list[0].from = 0;
    arg_list[0].th = 0;
    arg_list[0].count = COUNT/2;
    arg_list[1].from = arg_list[0].count;
    arg_list[1].count = arg_list[0].count;
    arg_list[1].th = 1;

    pthread_create(&thread, NULL, map, arg_list);
    map((void *)(arg_list + 1));
    pthread_join(thread, NULL);
    
    printf("%f", reduce() * 4.0);

    return 0;
}
