#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct shmem
{
	int key;
	uint64 pa[NPAB];
	int refcount;
	int size;
	struct spinlock lock;
};

struct shmem shmemtable[NSHM];

void shm_init(void)
{
	for(int i = 0; i < NSHM; i++){
		shmemtable[i].key = -1;
		shmemtable[i].refcount = 0;
		shmemtable[i].size = 0;
		initlock(&shmemtable[i].lock, "shmem");
	}
}

int shm_get(int key, int size, void **addr)
{
	struct shmem *shm = 0;
	for(struct shmem *curr_shm = shmemtable; curr_shm < &shmemtable[NSHM]; curr_shm++){
		// duplicated key
		if(curr_shm->key == key && curr_shm->refcount > 0){
			if (shm)
				release(&shm->lock);

			shm = curr_shm;
			break;
		}
		// find a free position in the table
		if(!shm && curr_shm->refcount == 0){
			acquire(&curr_shm->lock);
			shm = curr_shm;
			// break;?
		}
	}

	// shared memory table is full
	if (!shm)
		return -1;

	struct proc *p = myproc();
	int shmd = -1;
	struct procshm *pshm = 0;

	for(int i = 0; i < NSHMPROC; i++){
		// find a free prosition
		acquire(&p->oshm[i]->lock);
		// Esta es la manera correcta de ver si esta libre la posicion en el arreglo del proceso?
		if(!p->oshm[i]->shm){
			shmd = i;
			pshm = p->oshm[i];
			break;
		}
		release(&p->oshm[i]->lock);
	}

	if(shmd == -1){
		release(&shm->lock);
		return -1;
	}

	int pa;
	uint64 oldsize = p->sz;
	int i = 0;

	for(int a = oldsize; a < p->sz; a += PGSIZE){
		if(i >= NPAB){	// Chequeo previo.
			release(&shm->lock);
			release(&pshm->lock);
			return -1;
		}
		// Esto nos permite sacar el uvmalloc de más arriba.
		// if (refcount == 0) {
			// pa = kalloc();
			// shm->pa[i] = pa;
		//}
		// else
			// pa = shm->pa[i];

		// mappages(p->pagetable, a, PGSIZE, pa, PTE_R|PTE_U|PTE_W);
		i++;
		p->sz+=PGSIZE;
	}
	shm->refcount++;

	pshm->va = (uint64) &addr;
	pshm->shm = shm; 
	//p->oshm[shmd]->va = (uint64) &addr;void*a

	//p->oshm[shmd]->shm = shm;
	release(&shm->lock);
	release(&pshm->lock);

	copyout(p->pagetable, addr, &oldsize, sizeof(oldsize));
	return shmd;
}

int shm_close(int shm)
{
	return 0;
}
