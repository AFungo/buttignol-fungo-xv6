#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define QSZ 10

struct queue{
    int addr[QSZ];
    int first;
    int last;
    int mutex;
}queue;

void queue_init(struct queue *q){
    q->first = -1;
    q->last = 0;
    q->mutex = semcreate(2, 1);
}

void enqueue(struct queue *q, int n) {
    semwait(q->mutex);
    if (q->first == -1)
        q->first++;
    q->addr[q->last] = n;
    q->last = (q->last + 1) % QSZ;
    semsignal(q->mutex);
}

int dequeue(struct queue *q) {
    semwait(q->mutex);
    int val = q->addr[q->first];
    q->first = (q->first + 1) % QSZ;
    semsignal(q->mutex);
    return val;
}

#endif