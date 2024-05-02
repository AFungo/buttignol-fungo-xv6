#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int i = 1;
  int priority, j, k, x = 0;  
  for (priority = 1; priority < MAXLEVELS; priority++) {
    for (j = 0; j < 3; j++) {
      if (fork() == 0) {
        set_priority(priority - 1);
        for(k = 0; k<100000; k++){
          set_priority(priority - 1);
          x++;
        }
        exit(0);
      }
      i++;
    }
  }  
  fprintf(2,"parent: %d\n", getpid());  
  for(j = 0; j<9; j++){
    int pid = wait(0);
    fprintf(2, "%d\n", pid);
  }  
  return 0;
}
