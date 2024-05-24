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
	for(int i = 0; i < NSEM; i++)
	{
		semtable[i].value = 0;
		semtable[i].key = -1;
		semtable[i].ref_count = 0;
		initlock(&semtable[i].lock, "semaphore");
	}
}

int
alloc_sem_proc(struct proc* p)
{
	int i;
	for(i = 0; i<NSEMP && p->osems[i] != 0; i++);
	
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

	struct proc* p = myproc();
	//semaphore descriptor
	int sd = alloc_sem_proc(p);
	//process semaphore table is full
	if(sd == -1) 
		return -1;

	struct semaphore *free_sem = 0;
	for(struct semaphore *sem = semtable; sem < &semtable[NSEM]; sem++)
	{	
		//duplicated key
		if(sem->key == key && sem->ref_count > 0)
		{
			if (free_sem) {
				release(&free_sem->lock);
			}
			return -1;
		}
		if(!free_sem && sem->ref_count == 0){
			acquire(&sem->lock);
			free_sem = sem; 
		}
	}
  	//semaphore table is full
	if(!free_sem) 
		return -1;

	free_sem->value = value;
	free_sem->key = key;
	free_sem->ref_count = 1;
	p->osems[sd] = free_sem;

	release(&free_sem->lock);

	return sd;
}

int
semget(int key)
{
	struct proc* p = myproc();
	int sd = alloc_sem_proc(p);
	if (sd == -1)
		return -1;
	
	for(int i = 0; i < NSEMP; i++)
	{
		struct semaphore* currentsem = &semtable[i];
		acquire(&currentsem->lock);
		if(currentsem->key == key && currentsem->ref_count > 0){
			currentsem->ref_count++;
			p->osems[sd] = currentsem;
			release(&currentsem->lock);
			return sd;
		}
		release(&currentsem->lock);
	}
	return -1;
}

int
semsignal(int sd) {
	if(sd < 0 || sd >= NSEMP)
		return -1;
	struct semaphore *sem = myproc()->osems[sd];
	//check if semaphore exists
	if(!sem)
		return -1;
	acquire(&sem->lock);
	sem->value++;
	release(&sem->lock);
	wakeup(sem);
	return 0;
}

int
semwait(int sd)
{	
	if(sd < 0 || sd >= NSEMP)
		return -1;
	struct semaphore *sem = myproc()->osems[sd];
	//check if semaphore exists
	if(!sem)
		return -1;
	acquire(&sem->lock);
	while(sem->value == 0)
		sleep(sem, &sem->lock);
	sem->value--;
	release(&sem->lock);
	return 0;
}

int
semclose(int sd)
{
	if(sd < 0 || sd >= NSEMP)
		return -1;

	struct proc * p = myproc();
	struct semaphore *sem = p->osems[sd];
	//check if semaphore exists
	if(!sem)
		return -1;
	
	acquire(&sem->lock);
	sem->ref_count--;
	release(&sem->lock);
	
	p->osems[sd] = 0;
	return 0;
}
