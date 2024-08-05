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
	int size;	//npages
	struct spinlock lock;
};

struct shmem shmemtable[NSHM];

void shminit(void)
{
	for(int i = 0; i < NSHM; i++){
		shmemtable[i].key = -1;
		shmemtable[i].refcount = 0;
		shmemtable[i].size = 0;
		initlock(&shmemtable[i].lock, "shmem");
	}
}

struct shmem* find_shm(int key){
	
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
			// break;
		}
	}
	return shm;

}

int alloc_procshm(struct procshm* pshm){

	struct proc *p = myproc();
	int shmd = -1;
	for(int i = 0; i < NSHMPROC; i++){
		// find a free prosition
		struct procshm oshm = p->oshm[i];
		if(!oshm.shm){
			shmd = i;
			*pshm = p->oshm[i];
			break;
		}
	}

	return shmd;
}

int shm_get(int key, int size, void **addr)
{
	int npages = PGROUNDUP(size)/PGSIZE;
	if(npages > NPAB)
		return -1;

	struct procshm pshm;
	int shmd = alloc_procshm(&pshm);
	if(shmd == -1)
		return -1;
	
	struct shmem *shm = find_shm(key);
	if (!shm)
		return -1;
	
	struct proc *p = myproc();
	uint64 pa;
	uint64 oldsize = p->sz;
	int i = 0;	
	for(int a = oldsize; a < size+oldsize; a += PGSIZE){
		if (shm->refcount == 0) {
			pa = (uint64) kalloc();
			if(pa == 0){
				p->sz = uvmdealloc(p->pagetable, a, oldsize);
				release(&shm->lock);
				return -1;
			}
			shm->pa[i] = (uint64) pa;
		}else{
			pa = shm->pa[i];
		}
		if(mappages(p->pagetable, a, PGSIZE, pa, PTE_R|PTE_U|PTE_W) != 0){
			kfree((void*) pa);
			uvmdealloc(p->pagetable, a, oldsize);
			return -1;
		}
		i++;
		p->sz+=PGSIZE;
	}
	shm->size = npages;
	shm->refcount++;
	release(&shm->lock);
	//Se rompe cuando hace *addr dentro de copyout
	copyout(p->pagetable, (uint64)addr, (char*)oldsize, sizeof(oldsize));
	pshm.va = (uint64) addr;
	pshm.shm = shm;
	return shmd;
}

int shm_close(int shm)
{
	if(shm < 0 || shm >= NSHMPROC)
		return -1;

	struct proc *p = myproc();
	struct procshm *pshm = &(p->oshm[shm]);
	acquire(&pshm->shm->lock);
	if(pshm->shm == 0)
		return -1;
	pshm->shm->refcount--;

	int do_free = 0;
	if (pshm->shm->refcount == 0) {
		do_free = 1;
	}
	uvmunmap(p->pagetable, pshm->va, pshm->shm->size, do_free);
	pshm->shm = 0;

	return 0;
}