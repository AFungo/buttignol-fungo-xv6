#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {    
    int* addr;
    int shmd = shm_get(5, 4096, (void **)&addr);
    
    if (shmd < 0) {
        printf("Failed to get shared memory.\n");
        exit(1);
    }

    printf("Shared memory id: %d\n", shmd);
    printf("Shared memory address: %p\n", addr);

    // Store values in the scharhared memory
    addr[0] = 42;     // Store an integer value

    if (fork() == 0) {
        int* address; // Use a single pointer for the 
        shm_get(5, 4096, (void**) &address); // Get the shared memory ID and map the memory
        printf("Stored value TEST 2: %d\n", *address);

        address[0] = 10;
        printf("NEW STORED VALUE: %d\n", *address);
        exit(0);
    }
    wait(0);
    printf("Stored value: %d\n", *addr);


    return 0;
}
