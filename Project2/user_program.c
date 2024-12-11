#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

#define NUM_THREADS 10

void *enter_wait_queue(void *thread_id) {
    fprintf(stderr, "enter wait queue thread_id: %d\n", *(int *)thread_id);
    if (syscall(450, 1) == 0) {
        perror("syscall enter_wait_queue failed");
    }
    fprintf(stderr, "exit wait queue thread_id: %d\n", *(int *)thread_id);
    pthread_exit(NULL); 
}

void *clean_wait_queue() {
    if (syscall(450, 2) == 0) {
        perror("syscall clean_wait_queue failed");
    }
    return NULL;
}

int main() {
    void *ret;
    pthread_t id[NUM_THREADS];
    int thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        if (pthread_create(&id[i], NULL, enter_wait_queue, (void *)&thread_args[i]) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }

    sleep(1); 
    fprintf(stderr, "start clean queue ...\n");
    clean_wait_queue();

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(id[i], &ret) != 0) {
            perror("pthread_join failed");
            exit(1);
        }
    }
    return 0;
}
