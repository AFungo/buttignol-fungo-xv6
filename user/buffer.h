#ifndef _BUFFER_H_
#define _BUFFER_H_

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

void buffer_init(struct buffer *buff){
    buff->first = -1;
    buff->last = 0;
    buff->is_empty = semcreate(0, 0);
    buff->is_full = semcreate(1, BFSZ);
    buff->mutex = semcreate(2, 1);
}

void enqueue(struct buffer *bf, int n) {
    semwait(bf->is_full);
    semwait(bf->mutex);
    if (bf->first == -1)
        bf->first++;
    bf->addr[bf->last] = n;
    bf->last = (bf->last + 1) % BFSZ;
    semsignal(bf->mutex);
    semsignal(bf->is_empty);
}

int dequeue(struct buffer *bf) {
    semwait(bf->is_empty);
    semwait(bf->mutex);
    int val = bf->addr[bf->first];
    bf->first = (bf->first + 1) % BFSZ;
    semsignal(bf->mutex);
    semsignal(bf->is_full);
    return val;
}

void getbuffer(struct buffer *bf, int key){
    shm_get(key, sizeof(struct buffer), (void**)&bf);
    bf->is_empty = semget(0);
    bf->is_full = semget(1);
    bf->mutex = semget(2);
}

#endif