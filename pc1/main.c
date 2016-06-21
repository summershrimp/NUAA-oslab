#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

pthread_cond_t buffer1_change = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buffer1_mutex = PTHREAD_MUTEX_INITIALIZER;
int buffer1_full = 0;
pthread_cond_t buffer2_change = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buffer2_mutex = PTHREAD_MUTEX_INITIALIZER;
int buffer2_full = 0;

char buffer1[4];
char buffer2[4];

void *producer(void *args)
{
    pthread_mutex_lock(&buffer1_mutex);
    for (int i=0; i<4; i++)
    {
        buffer1[i] = 'a' + i;
    }
    buffer1_full = 1;
    pthread_cond_signal(&buffer1_change);
    while(buffer1_full)
        pthread_cond_wait(&buffer1_change, &buffer1_mutex);

    for (int i=0; i<4; i++)
    {
        buffer1[i] = 'e' + i;
    }
    buffer1_full = 1;
    pthread_cond_signal(&buffer1_change);
    while(buffer1_full)
        pthread_cond_wait(&buffer1_change, &buffer1_mutex);
    return NULL;
}

void *calcutor(void *args)
{
    pthread_mutex_lock(&buffer1_mutex);
    while(1)
    {
        while(!buffer1_full)
            pthread_cond_wait(&buffer1_change, &buffer1_mutex);
        pthread_mutex_lock(&buffer2_mutex);
        for(int i=0; i<4; ++i)
        {
            buffer2[i] = toupper(buffer1[i]);
        }
        buffer2_full = 1;
        pthread_cond_signal(&buffer2_change);
        while(buffer2_full)
            pthread_cond_wait(&buffer2_change, &buffer2_mutex);
        buffer1_full = 0;
        pthread_cond_signal(&buffer1_change);
        pthread_mutex_unlock(&buffer2_mutex);
    }
    return NULL;
}

void *consumer(void * args)
{
    pthread_mutex_lock(&buffer2_mutex);
    while(1)
    {
        while(!buffer2_full)
            pthread_cond_wait(&buffer2_change, &buffer2_mutex);
        for(int i=0;i<4; ++i)
        {
            printf("%c ", buffer2[i]);
        }
        buffer2_full = 0;
        pthread_cond_signal(&buffer2_change);
    }
    return NULL;
}


int main(void)
{
    pthread_t calc, cons;
    pthread_create(&calc,NULL, calcutor, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    producer(NULL);
    return 0;
}
