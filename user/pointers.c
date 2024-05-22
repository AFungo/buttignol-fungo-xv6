#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int global_var = 123;

int 
main(void) 
{
    int *a = malloc(1024);
    printf("Pointer to heap: %p\n", a);

    printf("Pointer to data segment: %p\n", &global_var);

    printf("Pointer to code segment: %p\n", main);

    printf("Pointer to stack: %p\n", &a);

    return 0;
}