#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

enum { THINKING, HUNGRY, EATING };

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[2];
int state[2];

int left(int i) { return (i + 2 - 1) % 2; }
int right(int i) { return (i + 1) % 2; }

void test(int i) {
    if (state[i] == HUNGRY &&
        state[left(i)] != EATING &&
        state[right(i)] != EATING) {
        state[i] = EATING;
        pthread_cond_signal(&cond[i]);
    }
}

void pickUp(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = HUNGRY;
    printf("Phil%d is hungry\n", i);

    test(i);
    while (state[i] != EATING)
        pthread_cond_wait(&cond[i], &mutex);

    printf("Phil%d picked up forks %d and %d\n", i, left(i), i);
    printf("Phil%d is eating\n", i);

    pthread_mutex_unlock(&mutex);
}

void putDown(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = THINKING;
    printf("Phil%d puts down forks %d and %d\n", i, left(i), i);

    test(left(i));
    test(right(i));

    pthread_mutex_unlock(&mutex);
}

void* philosopher(void* arg) {
    int phnum = *(int*)arg; 
    while (1) {
        printf("Phil%d is thinking\n", phnum);
        sleep(1);

        pickUp(phnum);
        sleep(2);

        putDown(phnum);
    }
    return NULL;
}

int main() {
    pthread_t threads[2];
    int philosopher_numbers[2];

    for (int i = 0; i < 2; ++i) {
        philosopher_numbers[i] = i;
        pthread_cond_init(&cond[i], NULL);
    }

    for (int i = 0; i < 2; ++i) {
        pthread_create(&threads[i], NULL, philosopher, (void*)&philosopher_numbers[i]);
    }

    for (int i = 0; i < 2; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
