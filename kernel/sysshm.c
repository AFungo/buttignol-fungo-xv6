#include "types.h"
#include "defs.h"

uint64
sys_shm_get(void)
{
    int key;
    int size;
    uint64 addr;
    argint(0, &key);
    argint(1, &size);
    argaddr(2, &addr);
    void *addr_ptr = (void *) addr;
    return shm_get(key, size, &addr_ptr);
}

uint64
sys_shm_close(void)
{
    return 0;
}