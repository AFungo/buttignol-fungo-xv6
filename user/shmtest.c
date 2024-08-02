#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
    void* addr = malloc(sizeof(uint64)); // Use a single pointer for the address
    int shmd = shm_get(5, 4096, &addr); // Get the shared memory ID and map the memory
    int* add = (int*)addr; // Dereference the pointer to get the address

    if (shmd < 0) {
        printf("Failed to get shared memory.\n");
        exit(1);
    }

    printf("Shared memory id: %d\n", shmd);
    printf("Shared memory address: %d\n", addr);

    // Store values in the scharhared memory
    add[0] = 42;     // Store an integer value
    printf("Stored value: %d\n", add[0]);

    exit(0);
}
