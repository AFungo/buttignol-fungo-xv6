#include "types.h"

uint64
sys_semcreate(void)
{
  int key;
  int value;
  argint(0, &key);
  argint(1, &value);
  return semcreate(key, value);
}


uint64
sys_semget(void)
{
  int key;
  argint(0, &key);
  return semget(key);
}



uint64
sys_semsignal(void)
{
  int sd;
  argint(0, &sd);
  return semsignal(sd);
}



uint64
sys_semwait(void)
{
  int sd;
  argint(0, &sd);
  return semwait(sd);
}



uint64
sys_semclose(void)
{
  int sd;
  argint(0, &sd);
  return semclose(sd);
}