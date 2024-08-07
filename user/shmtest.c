#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BFSZ 10

struct buffer{
    int addr[BFSZ];
    int first;
    int last;
    int is_empty;
    int is_full;
    int mutex;
}buffer;

void buffer_init(struct buffer *buff, int key){
    buff->first = -1;
    buff->last = 0;
    buff->is_empty = semcreate(0, 0);
    buff->is_full = semcreate(1, BFSZ);
    buff->mutex = semcreate(2, 1);
}

void enqueue(struct buffer *bf, int n) {
    semwait(bf->mutex);
    if (bf->first == -1)
        bf->first++;
    bf->addr[bf->last] = n;
    bf->last = (bf->last + 1) % BFSZ;
    semsignal(bf->mutex);
}

int dequeue(struct buffer *bf) {
    semwait(bf->mutex);
    int val = bf->addr[bf->first];
    bf->first = (bf->first + 1) % BFSZ;
    semsignal(bf->mutex);
    return val;
}

void getbuffer(struct buffer *bf){
    shm_get(4, sizeof(struct buffer), (void**)&bf);
    bf->is_empty = semget(0);
    bf->is_full = semget(1);
    bf->mutex = semget(2);
}

int main(void) {
    struct buffer *bf;
    shm_get(4, sizeof(struct buffer), (void**)&bf);
    buffer_init(bf, 5);
    printf("size = %d - %d\n", sizeof(struct buffer), sizeof(*bf));
    if (fork() == 0) {    
        // child process
        // consumer
        getbuffer(bf);
        for (int i = 0; i < BFSZ; i++) {
            semwait(bf->is_empty);
            // sleep(5);
            // printf("Dequeuing: %d\n", dequeue(bf));
            semsignal(bf->is_full);
        }
    } else {
        // parent process
        // producer
        for (int i = 0; i < BFSZ; i++) {
            semwait(bf->is_full);
            enqueue(bf, i);
            // printf("Enqueuing: %d\n", i);
            // sleep(5);
            semsignal(bf->is_empty);
        }
        wait(0);
    }
    return 0;
}