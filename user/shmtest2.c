#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void) {
    int* addr = 0; // Use a single pointer for the address
    shm_get(5, 4096, (void*) &addr); // Get the shared memory ID and map the memory

    printf("Stored value: %d\n", &addr);

    /* char* str = (char*)&addr + sizeof(int);
    printf("Stored string: %s\n", str); */
    exit(0);
}
