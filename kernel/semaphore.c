#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

struct semaphore{
  int value;
  int key;
  int ref_count;
  struct spinlock lock;
};

struct semaphore semtable[NSEM];

void
seminit(void)
{
	// for(struct semaphore *s = semtable;	s < &semtable[NSEM]; s++)
	for(int i = 0; i < NSEM; i++)
	{
		semtable[i].value = 0;
		semtable[i].key = -1;
		semtable[i].ref_count = 0;
		initlock(&semtable[i].lock, "semaphore");
	}
}

int
alloc_sem_proc()
{
	int i;
	for(i = 0; i<NSEMP && myproc()->osems[i] != 0; i++);
	
	//Process semaphore table is full
	if(i == NSEMP)
		return -1;
	return i;

}

int
semcreate(int key, int value)
{
	//key cant be negative
	if(key < 0)
		return -1;

	//value cant be negative
	if(value < 0)
		return -1;

	//semaphore descriptor
	int sd;
	if((sd = alloc_sem_proc()) == -1) 
		return -1;
	
	//ciclar hasta que haya un semaforo libre
	struct semaphore *free_sem = 0;
	for(struct semaphore *sem = semtable; sem < &semtable[NSEM]; sem++)
	{	
		//duplicated key
		if(sem->key == key)
		{
			if (free_sem != 0)
				release(&sem->lock);
			return -1;
		}
		if(free_sem == 0 && sem->ref_count == 0){
			acquire(&sem->lock);
			free_sem = sem;
		}
	}
  //semaphore table is full
	if(free_sem == 0) 
		return -1;

	free_sem->value = value;
	free_sem->key = key;
	free_sem->ref_count = 1;

	release(&free_sem->lock);
	myproc()->osems[sd] = free_sem;

	return sd;
}

int
semget(int key)
{
	struct proc* p = myproc();
	
	for(int i = 0; i < NSEMP; i++)
	{
		struct semaphore sem = semtable[i];
		acquire(&sem.lock);
		if(sem.key == key){
			int sd = alloc_sem_proc();
			p->osems[sd] = &sem;
		}
		release(&sem.lock);
	}
	return -1;
}

int
semsignal(int sem_idx) {
	if(sem_idx < 0 || sem_idx > NSEMP)
		return -1;
	struct semaphore *sem = myproc()->osems[sem_idx];
	//check if semaphore exists
	if(sem == 0)
		return -1;
	acquire(&sem->lock);
	sem->value++;
	release(&sem->lock);
	wakeup(sem);
	return 0;
}

int
semwait(int sem_idx)
{	
	if(sem_idx < 0 || sem_idx > NSEMP)
		return -1;
	struct semaphore *sem = myproc()->osems[sem_idx];
	//check if semaphore exists
	if(sem == 0)
		return -1;
	acquire(&sem->lock);//preguntar al marcelo
	while(sem->value == 0)
		sleep(sem, &sem->lock);
	sem->value--;
	release(&sem->lock);
	return 0;
}

int
semclose(int sem_idx)
{
	if(sem_idx < 0 || sem_idx > NSEMP)
		return -1;
	struct semaphore *sem = myproc()->osems[sem_idx];
	//check if semaphore exists
	if(sem == 0)
		return -1;
	
	acquire(&sem->lock);
	sem->ref_count--;
	release(&sem->lock);
	
	myproc()->osems[sem_idx] = 0;
	return 0;
}
