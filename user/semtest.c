#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Write on a file
void enqueue(int sd, int fd, int n) {
    semwait(sd);
    char c = (char) ('a' + n);
    write(fd, &c, 1);
    printf("Enqueuing: %d\n", n);
    semsignal(sd);
} 

// Read from a file
void dequeue(int sd, int fd) {
    char c;
    semwait(sd);
    read(fd, &c, 1);
    printf("Dequeuing %c\n", c);
    semsignal(sd);
}

int
main(void) 
{
    int isempty = semcreate(0, 0);
    int isfull = semcreate(1, 10);
    int mutex = semcreate(2, 1);

    if (fork() == 0) {
        isempty = semget(0);
        isfull = semget(1);
        mutex = semget(2);

        int fd = open("test.txt", O_RDONLY);
        for (int i = 0; i < 5; i++) {
            semwait(isempty);
            dequeue(mutex, fd);
            semsignal(isfull);
        }
        close(fd);

        exit(0);

    } else {

        int fd = open("test.txt", O_CREATE | O_WRONLY);
        for (int i = 0; i < 10; i++) {
            semwait(isfull);
            enqueue(mutex, fd, i);
            semsignal(isempty);
        }
        close(fd);
        wait(0);
        exit(0);
    }

    return 0;
}