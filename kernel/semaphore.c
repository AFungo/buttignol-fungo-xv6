#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

struct{
  int value;
  int key;
  int ref_count;
  spinlock lock;
  sleeplock slock;
}semaphore;



//can return (or not) descriptor 
static int
semcreate(int key, int value)
{	
	return 0;
}

static int 
semget(int key)
{
	return 0;
}

static int
semsignal(int sem) {
	return 0;
}

static int
semwait(int sem)
{
	return 0;
}

static int
semclose(int sem)
{
	return 0;
}
