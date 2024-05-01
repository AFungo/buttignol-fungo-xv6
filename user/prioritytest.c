#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 

void print(const char* s){
  write(1, s, strlen(s));
}

void prioritytest(void) {
  fork();
  print("process started with normal priority\n");
  set_priority(0);  
  print("Priority changed to highest\n");
  for(int i = 0; i < 4; i++) {
    if(fork() == 0){
      sleep(20);
      print("Child process running at highest priority\n");
      set_priority(0);
    }
  }
  set_priority(3);
  print("priority Set to lowest\n");
  sleep(10);
  set_priority(0);
  print("priority Set to Higest\n");  
}

int main(void){
  prioritytest();
  exit(0);
}

/*
void prioritytest(void){
    if(fork()==0){
      //set_priority(3);
      //sleep(4);
      for(;;){
				set_priority(3);
				print("Mensaje Terminado\n");
        set_priority(0);
				//sleep(4);
			}       
    }
		for(;;){
    	fork();
      //intf("\n %d soy el pid",getpid());
    	set_priority(0);
      set_priority(2);
      //print("\n soy un pid con prioridad 0\n");
		}
}
*/
