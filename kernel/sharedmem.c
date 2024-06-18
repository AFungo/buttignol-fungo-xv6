#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct shmem{
  struct spinlock lock;
};

struct shmem shmemtable[NSEM];

void
shm_init(void)
{

}

int
alloc_shm_proc(struct proc* p)
{

}

int
shm_get(int key, int size, void** addr)
{
    return 0;
}

int
shm_close(int shm)
{

}