#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
    int* addr = 0; // Use a single pointer for the address
    int shmd = shm_get(5, 4096, (void**) &addr); // Get the shared memory ID and map the memory

    if (shmd < 0) {
        printf("Failed to get shared memory.\n");
        exit(1);
    }

    printf("Shared memory id: %d\n", shmd);
    printf("Shared memory address: %p\n", &addr);

    // Store values in the scharhared memory
    *addr = 42;     // Store an integer value
    printf("Stored value: %d\n", addr[0]);

    exit(0);
}
