#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

typedef struct {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} sem_t;

int sem_init(sem_t *sem, int pshared, int value)
{
    pshared = 0;
    //pshared is no use
    sem->value = value;
    pthread_cond_init(&sem->cond, NULL);
    pthread_mutex_init(&sem->mutex, NULL);
    return 0;
}

int sem_wait(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    while(sem->value <= 0)
        pthread_cond_wait(&sem->cond, &sem->mutex);
    --sem->value;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

int sem_post(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex);
    ++sem->value;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

sem_t buf1_empty, buf1_full, buf2_empty, buf2_full;
pthread_mutex_t buf1_mutex=PTHREAD_MUTEX_INITIALIZER, buf2_mutex=PTHREAD_MUTEX_INITIALIZER;
char buffer1[4], buffer2[4];
int buf1_front = 0, buf1_rear = 0, buf2_front = 0, buf2_rear = 0;
void *producer(void *args)
{
    int i;
    for(i=0; i<8; ++i)
    {
        sem_wait(&buf1_empty);
        pthread_mutex_lock(&buf1_mutex);
        buffer1[buf1_rear++] = 'a' + i;
        buf1_rear %= 4;
        sem_post(&buf1_full);
        pthread_mutex_unlock(&buf1_mutex);
    }

    return NULL;
}

void *calculator(void *args)
{
    while(1)
    {
        sem_wait(&buf1_full);
        pthread_mutex_lock(&buf1_mutex);
        sem_wait(&buf2_empty);
        pthread_mutex_lock(&buf2_mutex);
        buffer2[buf2_rear++] = toupper(buffer1[buf1_front++]);
        buf2_rear %= 4;
        buf1_front %= 4;
        sem_post(&buf2_full);
        pthread_mutex_unlock(&buf2_mutex);
        sem_post(&buf1_empty);
        pthread_mutex_unlock(&buf1_mutex);
    }
    return NULL;
}

void *consumer(void *args)
{
    while(1)
    {
        sem_wait(&buf2_full);
        pthread_mutex_lock(&buf2_mutex);
        putchar(buffer2[buf2_front++]);
        putchar(' ');
        buf2_front %= 4;
        sem_post(&buf2_empty);
        pthread_mutex_unlock(&buf2_mutex);
    }
    return NULL;
}


int main(void)
{
    sem_init(&buf1_empty, 0 ,4);
    sem_init(&buf1_full, 0, 0);
    sem_init(&buf2_empty, 0, 4);
    sem_init(&buf2_full, 0, 0);

    pthread_t th_calc, th_cons;
    pthread_create(&th_calc, 0, calculator, NULL);
    pthread_create(&th_cons, 0, consumer, NULL);

    producer(NULL);
    usleep(100);
    return 0;
}

