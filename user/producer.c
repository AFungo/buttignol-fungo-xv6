#include "user/queue.h"

int main(void) {
    
    struct queue *q;
    if(shm_get(1, sizeof(struct queue), (void**)&q) == -1){
        printf("shm_get failed\n");
        exit(1);
    }
    int is_empty = semget(0);
    int is_full = semget(1);
    q->mutex = semget(2);

    for (int i = 0; i < QSZ; i++) {
        semwait(is_full);
        enqueue(q, i);
        printf("Enqueuing: %d\n", i);
        semsignal(is_empty);
        sleep(4);
    }

    exit(0);
}

