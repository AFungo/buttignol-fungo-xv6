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
        sleep(5);
        semwait(is_empty);
        printf("Dequeuing: %d\n", dequeue(q));
        semsignal(is_full);
    }

    exit(0);
}