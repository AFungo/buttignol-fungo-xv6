#include "user/buffer.h"

int main(void) {
    
    struct buffer *bf;
    if(shm_get(1, sizeof(struct buffer), (void**)&bf) == -1){
        printf("shm_get failed\n");
        exit(1);
    }
    bf->is_empty = semget(0);
    bf->is_full = semget(1);
    bf->mutex = semget(2);

    for (int i = 0; i < BFSZ; i++) {
        enqueue(bf, i);
        printf("Enqueuing: %d\n", i);
        sleep(40);
    }

    exit(0);
}

