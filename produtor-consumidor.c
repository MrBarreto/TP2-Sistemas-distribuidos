#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>
#include <time.h>

#define M (int)pow(10,5)

int *mem;
int N, consumidos;
int pos = 0;
int ocupacao_buffer[210000];

pthread_t *threads;
sem_t mutex, empty, full;

void gerar_numero(){
    for (int i=0; i<N; i++){
        if (mem[i]==0){
            mem[i] = (rand()%10000000) + 1;
            break;
        }
    }
    ocupacao_buffer[pos] = ocupacao_buffer[pos-1] + 1;
    pos++;
}
//%(int)pow(10,7)

void *produtor(){
    while(consumidos<M){
        sem_wait(&empty);
        sem_wait(&mutex);
        if (consumidos<M){
            gerar_numero();
        }
        sem_post(&mutex);
        sem_post(&full);
    }
    sem_post(&mutex);
    sem_post(&full);
    sem_post(&empty);
}

int ler_numero(){
    int numero, i;
    for (i=N-1; i >= 0; i--){
        if (mem[i]!=0){
            numero = mem[i];
            mem[i] = 0;
            break;
        }
    }
    consumidos++;
    ocupacao_buffer[pos] = ocupacao_buffer[pos-1] - 1;
    pos++;
    return numero;
} 


int primo(int numero){
    for (int i = 2; (i*i) <= numero; i++){
        if (numero % i == 0){
            return 0;
        }
    }
    return 1;
}

void *consumidor(){
    int numero;
    while(consumidos<M){
        sem_wait(&full);
        sem_wait(&mutex);
        if (consumidos<M){
            numero = ler_numero();
        }
        sem_post(&mutex);
        sem_post(&empty);
        if (primo(numero)){
            printf("%d é primo \n", numero);
        }
        else{
            printf("%d não é primo \n", numero);
        }
    }
    sem_post(&mutex);
    sem_post(&full);
    sem_post(&empty);
}

void criar_threads(int Np, int Nc){
    int i;

    for (i=0; i<Np; i++){
        pthread_create(&threads[i], NULL, &produtor, NULL);
    }
    for (i=Np; i<Np+Nc; i++){
        pthread_create(&threads[i], NULL, &consumidor, NULL);
    }
    for (i=0; i<Np+Nc; i++){
        pthread_join(threads[i], NULL);
    }
} 

int main(){
    int Np, Nc;
    struct timespec begin, end;
    double elapsed_time = 0;
    FILE * fp;
    srand(time(NULL));

    printf("Insira o tamanho do vetor: ");
    scanf("%i", &N);

    if (N!=1 && N!=10 && N!= 100 && N!=1000){
        printf("Valor inválido\n");
        return -1;
    }
    printf("Insira o número de produtores: ");
    scanf("%i", &Np);
    printf("Insira o número de consumidores: ");
    scanf("%i", &Nc);

    mem = (int *) malloc(N*sizeof(int));
    for(int i = 0; i<210000; i++){
            ocupacao_buffer[i] = - 1;
    }
    ocupacao_buffer[0] = 0;
    pos++;
    for(int iter=0; iter<10; iter++){
        consumidos = 0;
        pos = 1;
        sem_init(&mutex, 0, 1);
        sem_init(&empty, 0, N);
        sem_init(&full, 0, 0);
        
        threads = (pthread_t *) malloc((Np+Nc)*sizeof(pthread_t));
        clock_gettime(CLOCK_MONOTONIC, &begin);
        criar_threads(Np, Nc);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        free(threads);
        elapsed_time += (end.tv_sec - begin.tv_sec);
        elapsed_time += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

        sem_destroy(&mutex);
        sem_destroy(&full);
        sem_destroy(&empty);

        for (int i =0;i<N;i++){
            mem[i] = 0;
        }
        printf("Acabou\n");
    }
    printf("O tempo decorrido medio foi de %f \n", elapsed_time / 10);

    fp = fopen ("buffer.txt", "w");
    for (int o=0; o<210000; o++){
        fprintf(fp,"%i",ocupacao_buffer[o]);
        fprintf(fp, "%s", "\n");
    }
    fclose(fp);
    return 0;
}