#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"



struct shmem{
  int key;
  uint64 pa[NPAB];//TODO: cambiar/definir contaste
  int refcount;
  int size;
  struct spinlock lock;
};

struct shmem shmemtable[NSHM];

void
shm_init(void)
{
  for(int i = 0; i < NSHM; i++){
    shmemtable[i].key = -1;
    shmemtable[i].refcount = 0;
    shmemtable[i].size = 0;
    initlock(&shmemtable[i].lock, "shmem");
  }
}

int
alloc_shm_proc(struct proc* p)
{
  //map va to pa
}

int
shm_get(int key, int size, void** addr)
{

		//size = size * PGSIZE;//en el caso si te pasan el numero de bloques

		struct shmem *shm = 0;
		for(struct shmem *curr_shm = shmemtable; curr_shm < &shmemtable[NSHM]; curr_shm++){	
			//duplicated key
			if(curr_shm->key == key && curr_shm->refcount > 0){
				if (shm) {
					release(&shm->lock);
				}
				shm = curr_shm;
				break;
			}
			//find a free position in the table
			if(!shm && curr_shm->refcount == 0){
				acquire(&curr_shm->lock);
				shm = curr_shm;
			}
		}
		
		//shared memory table is full
		if(shm == 0){
			return -1;
		}

    struct proc *p = myproc();

		int shmd = -1;
		struct procshm *pshm = 0;	
	
		//find a free prosition 
    for(int i = 0; i < NSHMPROC; i++){
      acquire(&p->oshm[i]->lock);
			//Esta es la manera correcta de ver si esta libre la posicion en el arreglo del proceso?
      if(p->oshm[i]->va == 0){
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



    if(shm->refcount <= 0){

    	int pa;      
			int free = 0;      
					
			int oldsize = p->sz;			
			int newsize;
			if((newsize = uvmalloc(p->pagetable, oldsize, oldsize+size, PTE_W))==0){
				release(&shm->lock);
				release(&pshm->lock);
				return -1;
			}

			p->sz = newsize;
			
			int i = 0;

			for(int a = oldsize; a < p->sz; a += PGSIZE){
				if(i >= NPAB){
					release(&shm->lock);
					release(&pshm->lock);
					return -1;
				}
				uint64 *addr = (uint64)*addr
				*addr = PGROUNDDOWN(a);
				pa = walkaddr(p->pagetable, **addr);
				shm->pa[i] = pa;
				i++;
			}
			shm->ref_count++;
    }else{
			//if key was found

      *addr = PGROUNDDOWN(p->sz);
      int sz = shm->size;

      //TODO: aca iria un ciclo? para cada memoria fisica una virtual? las fisicas son contiguas?
      //for i : size
      pa = shm->pa[0];
      mappages(p->pagetable, *addr, size, pa, perm);
    }
    
		int shmd = -1;

		pshm->va = &addr;
    pshm->shmd_tableidx = shmd_table;//

		if(shmd != -1){
			p->shmproc[shmd].va = &addr;
			p->shmproc[shmd].shmem = shmtable[free];
		}
		
		&addr = va;		

		release(&shm->lock);
		release(&pshm->lock);

    return shmd;
}

int
shm_close(int shm)
{

}
