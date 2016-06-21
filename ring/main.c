//
//  main.c
//  pthread2
//
//  Created by Summer on 5/28/16.
//  Copyright © 2016 summer. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *calc(void *param);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t nmutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int n = 555;
unsigned int reg = 0;

unsigned int cnt;

int main(void)
{
    int i, *p;
    scanf("%d", &cnt);
    pthread_t *pt;
    for(i=0; i<cnt; ++i)
    {
        pt = malloc(sizeof(pthread_t));
        p = malloc(sizeof(int));
        *p = i;
        pthread_create(pt, NULL, calc, p);
    }
    pthread_join(*pt, NULL);
    return 0;
}

void *calc(void *param)
{
    int nth = *(int *)param;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(reg == nth)
        {
            reg++;
            
            pthread_mutex_lock(&nmutex);
            pthread_mutex_unlock(&mutex);
            if(reg == cnt)
                pthread_cond_signal(&cond);
            pthread_cond_wait(&cond, &nmutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    while(1)
    {
        ++n;
        printf("Calc %d: %d\n", nth, n);
        fflush(stdin);
        usleep(100);
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond, &nmutex);
    }
    
    return NULL;
}
