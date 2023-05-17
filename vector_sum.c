#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>
#include<math.h>

atomic_flag lock = ATOMIC_FLAG_INIT;

char *numbers;
int N;
int length;
long int acumulator;

void acquire()
{
    while (atomic_flag_test_and_set(&lock))
        ;
}

void release()
{
    atomic_flag_clear(&lock);
}
void *sum_fun(void *threadid)
{
    long int local_acumulator = 0;
    int slice = length / N;
    int rest = length % N;
    int begin = ((int)threadid) * slice;
    int last = begin + slice - 1;
    if ((int)threadid == N - 1)
    {
        last += rest;
    }
    for (int i = begin; i <= last; i++)
    {
        local_acumulator += numbers[i];
    }
    acquire();
    acumulator += local_acumulator;
    release();
}

void thread_creation(int *thids, pthread_t *threads)
{
    int resul;
    for (int t = 0; t < N; t++)
    {
        thids[t] = t;
        resul = pthread_create(&threads[t], NULL, sum_fun, (void *)thids[t]);
        if (resul)
        {
            printf("Falha na criação da thread, código %d\n", resul);
        }
    }
    for (int t = 0; t < N; t++)
    {
        pthread_join(threads[t], NULL);
    }
}

int main()
{
    int *thids;
    pthread_t *threads;
    srand(time(NULL));
    char r = (rand() % 201) -100;
    struct timespec begin, end;
    double elapsed_time = 0;
    long int veri = 0;
    printf("Entre com o número de threads: \n");
    scanf("%i", &N);
    printf("Entre com o tamanho do vetor: \n");
    scanf("%i", &length);
    numbers = (char *)malloc(length * sizeof(char));
    for (int i = 0; i < length; i++)
    {
        numbers[i] = r;
        veri += numbers[i];
    }
    printf("O valor somado é %i\n", veri);
    
    for (int i = 0; i < 10; i++)
    {
        int *thids = (int *)malloc(N * sizeof(int));
        pthread_t *threads = (pthread_t *)malloc(N * sizeof(pthread_t));
        clock_gettime(CLOCK_MONOTONIC, &begin);
        thread_creation(thids, threads);
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time += (end.tv_sec - begin.tv_sec);
        elapsed_time += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
        printf("Soma com threads: %i\n", acumulator);
        acumulator = 0;
        free(thids);
        free(threads);
    }
    printf("O tempo decorrido medio foi de %f", elapsed_time / 10);
    free(numbers);
    return 0;
}
