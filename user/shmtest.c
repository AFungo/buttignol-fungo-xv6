#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
    int* addr = 0; // Use a single pointer for the address
    int shmd = shm_get(5, 4096, &addr); // Get the shared memory ID and map the memory

    if (shmd < 0) {
        printf("Failed to get shared memory.\n");
        exit(1);
    }

    printf("Shared memory id: %d\n", shmd);
    printf("Shared memory address: %p\n", &addr);

    // Store values in the scharhared memory
    int* int_ptr = (int*)&addr;
    *int_ptr = 42; // Store an integer value
    printf("Stored value: %d\n", *int_ptr);

    // You can store more complex data structures as well
    // For example, storing a string
    char* str = (char*)&addr + sizeof(int);
    strcpy(str, "Hello, shared memory!");
    printf("Stored string: %s\n", str);

    if (fork() == 0) {
        shm_get(5, 4096, &addr); // Get the shared memory ID and map the memory
        int* int_ptr = (int*)&addr;
        printf("Stored value: %d\n", *int_ptr);

        char* str = (char*)&addr + sizeof(int);
        printf("Stored string: %s\n", str);
        exit(0);
    }
    wait(0);

    // Remember to properly synchronize access in a multi-process/multi-threaded environment

    exit(0);
}
