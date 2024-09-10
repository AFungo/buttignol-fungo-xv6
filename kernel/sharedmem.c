#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

// Shared memory structure
struct shmem
{
	int key;				// Global ID
	uint64 pa[NPAB];		// Array of physical addresses
	int refcount;	
	int size;				// npages
	struct spinlock lock;
};

// Shared memories global table
struct shmem shmemtable[NSHM];

// Initialize shmemtable from main
void shminit(void)
{
	for(int i = 0; i < NSHM; i++){
		shmemtable[i].key = -1;
		shmemtable[i].refcount = 0;
		shmemtable[i].size = 0;
		initlock(&shmemtable[i].lock, "shmem");
	}
}

// Find a shm by key or return a free shared memory
struct shmem* find_shm(int key)
{
	struct shmem *shm = 0;
	int i = 0;
	for(struct shmem *curr_shm = shmemtable; curr_shm < &shmemtable[NSHM]; curr_shm++, i++){
		// duplicated key
		if(curr_shm->key == key && curr_shm->refcount > 0){
			if (shm)
				release(&shm->lock);
			acquire(&curr_shm->lock);
			shm = curr_shm;
			break;
		}
		// find a free position in the table
		if(!shm && curr_shm->refcount == 0){
			acquire(&curr_shm->lock);
			shm = curr_shm;
			shm->key = key;
		}
	}
	return shm;
}

// Find a free space in processes oshm
int alloc_procshm(struct proc* p)
{
	for(int i = 0; i < NSHMPROC; i++){
		// find a free prosition
		struct procshm oshm = p->oshm[i];
		if(!oshm.shm){
			return i;
		}
	}
	return -1;
}

// Get/create a shared memory of a given size
int shm_get(int key, int size, uint64 addr)
{
	int npages = PGROUNDUP(size)/PGSIZE;
	if(npages > NPAB)
		return -1;

	struct proc *p = myproc();
	int shmd = alloc_procshm(p);
	if(shmd == -1)
		return -1;
	
	struct shmem *shm = find_shm(key);
	if (!shm)
		return -1;
	
	uint64 pa;
	uint64 oldsize = p->shmsz;
	for(int a = oldsize, i=0; a < size+oldsize; a += PGSIZE, i++){
		if (shm->refcount == 0) {
			//this case allocate physical memory
			pa = (uint64) kalloc();
			if(pa == 0){
				p->shmsz = uvmdealloc(p->pagetable, a, oldsize);
				release(&shm->lock);
				return -1;
			}
			shm->pa[i] = (uint64) pa;
		}else{
			//otherwise we can get it from shared memory
			pa = shm->pa[i];
		}

		// Map physical address to 'a'
		if(mappages(p->pagetable, a, PGSIZE, pa, PTE_R|PTE_U|PTE_W) != 0){
			kfree((void*) pa);
			uvmdealloc(p->pagetable, a, oldsize);
			return -1;
		}
		
		p->shmsz+=PGSIZE;
	}
	shm->size = npages;
	shm->refcount++;
	release(&shm->lock);
	// Copy from kernel space to user space
	copyout(p->pagetable, addr, (char*)&oldsize, sizeof(oldsize));
	p->oshm[shmd].va = oldsize;
	p->oshm[shmd].shm = shm;
	return shmd;
}

// Free a shared memory
int shm_close(int shm)
{
	if(shm < 0 || shm >= NSHMPROC)
		return -1;

	struct proc *p = myproc();
	acquire(&p->oshm[shm].shm->lock);
	
	if(p->oshm[shm].shm == 0)
		return -1;

	p->oshm[shm].shm->refcount--;

	int do_free = 0;
	if (p->oshm[shm].shm->refcount == 0) {
		// If shm dont have any other reference we have to free physical memory
		do_free = 1;
	}

	uvmunmap(p->pagetable, p->oshm[shm].va, p->oshm[shm].shm->size, do_free);
	release(&p->oshm[shm].shm->lock);
	p->oshm[shm].shm = 0;
	p->oshm[shm].va = 0;
	return 0;
}