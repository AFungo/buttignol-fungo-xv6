#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct shmem
{
	int key;
	uint64 pa[NPAB]; // TODO: cambiar/definir contaste
	int refcount;
	int size;
	struct spinlock lock;
};

struct shmem shmemtable[NSHM];

void shm_init(void)
{
	for (int i = 0; i < NSHM; i++)
	{
		shmemtable[i].key = -1;
		shmemtable[i].refcount = 0;
		shmemtable[i].size = 0;
		initlock(&shmemtable[i].lock, "shmem");
	}
}

int alloc_shm_proc(struct proc *p)
{
	// map va to pa
	return 0;
}

int shm_get(int key, int size, void **addr)
{

	// size = size * PGSIZE;//en el caso si te pasan el numero de bloques

	struct shmem *shm = 0;
	for (struct shmem *curr_shm = shmemtable; curr_shm < &shmemtable[NSHM]; curr_shm++)
	{
		// duplicated key
		if (curr_shm->key == key && curr_shm->refcount > 0)
		{
			if (shm)
			{
				release(&shm->lock);
			}
			shm = curr_shm;
			break;
		}
		// find a free position in the table
		if (!shm && curr_shm->refcount == 0)
		{
			acquire(&curr_shm->lock);
			shm = curr_shm;
		}
	}

	// shared memory table is full
	if (shm == 0)
	{
		return -1;
	}

	struct proc *p = myproc();

	int shmd = -1;
	struct procshm *pshm = 0;

	// find a free prosition
	for (int i = 0; i < NSHMPROC; i++)
	{
		acquire(&p->oshm[i]->lock);
		// Esta es la manera correcta de ver si esta libre la posicion en el arreglo del proceso?
		if (p->oshm[i]->va == 0)
		{
			shmd = i;
			pshm = p->oshm[i];
			break;
		}
		release(&p->oshm[i]->lock);
	}

	if (shmd == -1)
	{
		release(&shm->lock);
		return -1;
	}

	if (shm->refcount <= 0)
	{
		int pa;
		int oldsize = p->sz;
		int newsize;

		if ((newsize = uvmalloc(p->pagetable, oldsize, oldsize + size, PTE_W)) == 0)
		{
			release(&shm->lock);
			release(&pshm->lock);
			return -1;
		}

		p->sz = newsize;
		int i = 0;
		*addr = (void*)(uint64) PGROUNDDOWN(oldsize);	//TODO: REVISAR ESTE CAST

		for (int a = oldsize; a < p->sz; a += PGSIZE)
		{
			if (i >= NPAB)
			{
				release(&shm->lock);
				release(&pshm->lock);
				return -1;
			}
			uint64 va = PGROUNDDOWN(a);
			pa = walkaddr(p->pagetable, va);
			shm->pa[i] = pa;
			i++;
		}
		shm->refcount++;
	}
	else
	{
		// if key was found
		int pa;
		*addr = (void*)(uint64) PGROUNDDOWN(p->sz);
		int sz = shm->size;

		// TODO: aca iria un ciclo? para cada memoria fisica una virtual? las fisicas son contiguas?
		// for i : size
		pa = shm->pa[0];
		mappages(p->pagetable, (uint64)*addr, sz, pa, PTE_W);
	}

	pshm->va = (uint64) &addr;
	pshm->shm = shm; 
	p->oshm[shmd]->va = (uint64) &addr;
	p->oshm[shmd]->shm = shm;
	release(&shm->lock);
	release(&pshm->lock);

	return shmd;
}

int shm_close(int shm)
{
	return 0;
}
