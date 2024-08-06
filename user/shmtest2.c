#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
    int* addr; // Use a single pointer for the 
    shm_get(5, 4096, (void**) &addr); // Get the shared memory ID and map the memory

    printf("Stored value TEST 2: %d\n", *addr);
    addr[0] = 10;
    printf("NEW STORED VALUE: %d\n", *addr);

    return 0;
}
