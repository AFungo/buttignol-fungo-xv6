#include "user/buffer.h"

#define BFSZ 10

int main(void) {
    struct buffer *bf;
    if(shm_get(4, sizeof(struct buffer), (void**)&bf) == -1){
        printf("shm_get failed\n");
        exit(1);
    }
    buffer_init(bf);
    if (fork() == 0) {    
        // child process
        // consumer
        if(shm_get(4, sizeof(struct buffer), (void**)&bf) == -1){
            printf("shm_get failed\n");
            exit(1);
        }
        bf->is_empty = semget(0);
        bf->is_full = semget(1);
        bf->mutex = semget(2);
        for (int i = 0; i < BFSZ; i++) {
            sleep(5);
            printf("Dequeuing: %d\n", dequeue(bf));
        }
    } else {
        // parent process
        // producer
        for (int i = 0; i < BFSZ; i++) {
            enqueue(bf, i);
            printf("Enqueuing: %d\n", i);
            sleep(5);
        }
        wait(0);
    }
    return 0;
}