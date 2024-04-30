#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 

void prioritytest(void){
    if(fork()==0){
        set_priority(3);       
    }
    printf("\n %d soy el pid",getpid());
    set_priority(0);
    printf("\n %d soy un pid con prioridad 0", getpid());
}

int main(void){
    prioritytest();
    exit(0);
}
