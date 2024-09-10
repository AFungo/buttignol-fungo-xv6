#include "user/queue.h"

int main(void) {
    struct queue *q;
    if(shm_get(1, sizeof(struct queue), (void**)&q) == -1){
        printf("shm_get failed\n");
        exit(1);
    }
    queue_init(q);
    semcreate(0, 0);
    semcreate(1, QSZ);
    if (fork() == 0) {
        char* args1[] = {"producer", 0};
        exec("producer", args1);
        exit(0);
    } else {
        if (fork() == 0) {
            char* args2[] = {"consumer", 0};
            exec("consumer", args2);
            exit(0);
        } 
    }
    wait(0);
    wait(0);
    return 0;
}